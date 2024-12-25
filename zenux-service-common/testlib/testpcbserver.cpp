#include "testpcbserver.h"
#include "mockserverparamgenerator.h"
#include <QFinalState>
#include <QDir>

TestPcbServer::TestPcbServer(QString serviceName,
                             VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory) :
    PCBServer(std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams(serviceName)),
              tcpNetworkFactory)
{
    m_pInitializationMachine = new QStateMachine(this);

    QState* stateCONF = new QState();
    QFinalState* stateFINISH = new QFinalState();

    QState* statesetupServer = new QState(stateCONF);
    m_stateconnect2RM = new QState(stateCONF);
    m_stateSendRMIdentAndRegister = new QState(stateCONF); // we send ident. to rm and register our resources

    stateCONF->setInitialState(statesetupServer);
    
    statesetupServer->addTransition(this, &TestPcbServer::sigServerIsSetUp, m_stateconnect2RM);

    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);
    
    QObject::connect(statesetupServer, &QAbstractState::entered, this, &TestPcbServer::doSetupServer);
    QObject::connect(m_stateconnect2RM, &QAbstractState::entered, this, &TestPcbServer::doConnect2RM);
    QObject::connect(m_stateSendRMIdentAndRegister, &QAbstractState::entered, this, &TestPcbServer::doIdentAndRegister);
}

TestPcbServer::~TestPcbServer()
{
    if (m_pInitializationMachine) delete m_pInitializationMachine;
    if (m_pRMConnection) delete m_pRMConnection;
}

void TestPcbServer::setXmlSettings(XmlSettingsList xmlSettings)
{
    m_xmlSettings = xmlSettings;
    doConfiguration();
}

void TestPcbServer::setResources(ResourcesList resources)
{
    m_resources = resources;
}

void TestPcbServer::setScpiConnections(ScpiConnectionList scpiConnections)
{
    m_scpiConnecttionsAddedFromExtern = scpiConnections;
}

Zera::XMLConfig::cReader *TestPcbServer::getConfigReader()
{
    return &m_xmlConfigReader;
}

RMConnection *TestPcbServer::getRmConnection()
{
    return m_pRMConnection;
}

void TestPcbServer::start()
{
    m_pInitializationMachine->start();
}

void TestPcbServer::doConfiguration()
{
    ServerParams params = m_settings->getServerParams();
    if (m_xmlConfigReader.loadSchema(params.xsdFile)) {
        EthSettings *ethSettings = m_settings->getEthSettings();
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
                ethSettings, &EthSettings::configXMLInfo);
        for (const auto &setting : qAsConst(m_xmlSettings))
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
                    setting, &XMLSettings::configXMLInfo);
        if (!m_xmlConfigReader.loadXMLFile(params.xmlFile))
            qFatal("Could not load xml config file");
        m_pRMConnection = new RMConnection(ethSettings->getRMIPadr(),
                                           ethSettings->getPort(EthSettings::resourcemanager),
                                           m_tcpNetworkFactory);
    }
    else
        qFatal("Could not load xml schema");
}

void TestPcbServer::doSetupServer()
{
    setupServer();
    scpiConnectionList.append(this); // the server itself has some commands
    for(const auto &resource : qAsConst(m_resources)) {
        scpiConnectionList.append(resource);
        resourceList.append(resource);
    }
    for(const auto &scpiConnection : qAsConst(m_scpiConnecttionsAddedFromExtern))
        scpiConnectionList.append(scpiConnection);
    initSCPIConnections();

    EthSettings *ethSettings = m_settings->getEthSettings();
    m_protoBufServer->startServer(ethSettings->getPort(EthSettings::protobufserver)); // and can start the server now

    m_stateconnect2RM->addTransition(m_pRMConnection, &RMConnection::connected, m_stateSendRMIdentAndRegister);

    emit sigServerIsSetUp();
}

void TestPcbServer::doConnect2RM()
{
    m_pRMConnection->connect2RM();
}

void TestPcbServer::doIdentAndRegister()
{
    m_pRMConnection->SendIdent(getName());
    EthSettings *ethSettings = m_settings->getEthSettings();
    for (int i = 0; i < resourceList.count(); i++) {
        cResource *res = resourceList.at(i);
        connect(m_pRMConnection, &RMConnection::rmAck, res, &cResource::resourceManagerAck);
        res->registerResource(m_pRMConnection, ethSettings->getPort(EthSettings::protobufserver));
    }
}
