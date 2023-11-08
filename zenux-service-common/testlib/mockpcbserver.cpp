#include "mockpcbserver.h"
#include "scpisingletonfactory.h"
#include <QFinalState>

MockPcbServer::MockPcbServer(QString serviceName) :
    cPCBServer(createParams(serviceName), ScpiSingletonFactory::getScpiObj())
{
    m_pInitializationMachine = new QStateMachine(this);

    QState* stateCONF = new QState();
    QFinalState* stateFINISH = new QFinalState();

    QState* statexmlConfiguration = new QState(stateCONF);
    QState* statesetupServer = new QState(stateCONF);
    m_stateconnect2RM = new QState(stateCONF);
    m_stateSendRMIdentAndRegister = new QState(stateCONF); // we send ident. to rm and register our resources

    stateCONF->setInitialState(statexmlConfiguration);

    statexmlConfiguration->addTransition(&m_xmlConfigReader, &Zera::XMLConfig::cReader::finishedParsingXML, statesetupServer);
    statesetupServer->addTransition(this, &MockPcbServer::sigServerIsSetUp, m_stateconnect2RM);

    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);

    QObject::connect(statexmlConfiguration, &QAbstractState::entered, this, &MockPcbServer::doConfiguration);
    QObject::connect(statesetupServer, &QAbstractState::entered, this, &MockPcbServer::doSetupServer);
    QObject::connect(m_stateconnect2RM, &QAbstractState::entered, this, &MockPcbServer::doConnect2RM);
    QObject::connect(m_stateSendRMIdentAndRegister, &QAbstractState::entered, this, &MockPcbServer::doIdentAndRegister);
}

MockPcbServer::~MockPcbServer()
{
    if (m_pInitializationMachine) delete m_pInitializationMachine;
    if (m_pRMConnection) delete m_pRMConnection;
}

void MockPcbServer::setResourcesWithConfig(QList<TResourceWithSettings> resourceWithSettings)
{
    m_resourceWithSettings = resourceWithSettings;
}

Zera::XMLConfig::cReader *MockPcbServer::getConfigReader()
{
    return &m_xmlConfigReader;
}

ServerParams MockPcbServer::createParams(QString serviceName)
{
    ServerParams params {
        QStringLiteral("Mock") + serviceName,
        "V42.0",
        QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + serviceName + ".xsd",
        QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + serviceName + ".xml"
    };
    return params;
}

void MockPcbServer::start()
{
    m_pInitializationMachine->start();
}

bool MockPcbServer::areAllResourcesConnected()
{
    return m_resourcesRegistered;
}

void MockPcbServer::doConfiguration()
{
    if (m_xmlConfigReader.loadSchema(m_params.xsdFile)) {
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
                &m_ethSettings, &EthSettings::configXMLInfo);
        for(const auto &resSetting : qAsConst(m_resourceWithSettings))
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
                    resSetting.settings, &XMLSettings::configXMLInfo);
        if (!m_xmlConfigReader.loadXMLFile(m_params.xmlFile))
            qFatal("Could not load xml config file");
    }
    else
        qFatal("Could not load xml schema");
}

void MockPcbServer::doSetupServer()
{
    setupServer();
    scpiConnectionList.append(this); // the server itself has some commands
    for(const auto &resSetting : qAsConst(m_resourceWithSettings)) {
        scpiConnectionList.append(resSetting.resource);
        resourceList.append(resSetting.resource);
    }
    initSCPIConnections();

    myServer->startServer(m_ethSettings.getPort(EthSettings::protobufserver)); // and can start the server now
    if(m_ethSettings.isSCPIactive())
        m_pSCPIServer->listen(QHostAddress::AnyIPv4, m_ethSettings.getPort(EthSettings::scpiserver));

    m_pRMConnection = new RMConnection(m_ethSettings.getRMIPadr(), m_ethSettings.getPort(EthSettings::resourcemanager));
    m_stateconnect2RM->addTransition(m_pRMConnection, &RMConnection::connected, m_stateSendRMIdentAndRegister);

    emit sigServerIsSetUp();
}

void MockPcbServer::doConnect2RM()
{
    m_pRMConnection->connect2RM();
}

void MockPcbServer::doIdentAndRegister()
{
    m_pRMConnection->SendIdent(getName());
    m_resourcesToConnect = resourceList.count();
    for (int i = 0; i < m_resourcesToConnect; i++) {
        cResource *res = resourceList.at(i);
        connect(m_pRMConnection, &RMConnection::rmAck, res, &cResource::resourceManagerAck);
        connect(res, &cResource::registerRdy, this, &MockPcbServer::onResourceRegisterRdy);
        res->registerResource(m_pRMConnection, m_ethSettings.getPort(EthSettings::protobufserver));
    }
}

void MockPcbServer::onResourceRegisterRdy()
{
    m_resourcesToConnect--;
    if(m_resourcesToConnect == 0)
        m_resourcesRegistered = true;
}
