#include "testpcbserver.h"
#include "serverparamgenerator.h"
#include "scpisingletonfactory.h"
#include <QFinalState>
#include <QDir>

TestPcbServer::TestPcbServer(QString serviceName) :
    cPCBServer(ServerParamGenerator::createParams(serviceName), ScpiSingletonFactory::getScpiObj())
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
    if (m_xmlConfigReader.loadSchema(m_params.xsdFile)) {
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
                &m_ethSettings, &EthSettings::configXMLInfo);
        for (const auto &setting : qAsConst(m_xmlSettings))
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
                    setting, &XMLSettings::configXMLInfo);
        if (!m_xmlConfigReader.loadXMLFile(m_params.xmlFile))
            qFatal("Could not load xml config file");
        m_pRMConnection = new RMConnection(m_ethSettings.getRMIPadr(), m_ethSettings.getPort(EthSettings::resourcemanager));
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

    m_myServer->startServer(m_ethSettings.getPort(EthSettings::protobufserver)); // and can start the server now
    if(m_ethSettings.isSCPIactive())
        m_pSCPIServer->listen(QHostAddress::AnyIPv4, m_ethSettings.getPort(EthSettings::scpiserver));

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
    for (int i = 0; i < resourceList.count(); i++) {
        cResource *res = resourceList.at(i);
        connect(m_pRMConnection, &RMConnection::rmAck, res, &cResource::resourceManagerAck);
        res->registerResource(m_pRMConnection, m_ethSettings.getPort(EthSettings::protobufserver));
    }
}
