#include "mockpcbserver.h"
#include "scpisingletonfactory.h"
#include <QFinalState>
#include <QDir>

MockPcbServer::MockPcbServer(QString serviceName, QString sysrootDir) :
    cPCBServer(createParams(serviceName, sysrootDir), ScpiSingletonFactory::getScpiObj())
{
    m_pInitializationMachine = new QStateMachine(this);

    QState* stateCONF = new QState();
    QFinalState* stateFINISH = new QFinalState();

    QState* statesetupServer = new QState(stateCONF);
    m_stateconnect2RM = new QState(stateCONF);
    m_stateSendRMIdentAndRegister = new QState(stateCONF); // we send ident. to rm and register our resources

    stateCONF->setInitialState(statesetupServer);

    statesetupServer->addTransition(this, &MockPcbServer::sigServerIsSetUp, m_stateconnect2RM);

    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);

    QObject::connect(statesetupServer, &QAbstractState::entered, this, &MockPcbServer::doSetupServer);
    QObject::connect(m_stateconnect2RM, &QAbstractState::entered, this, &MockPcbServer::doConnect2RM);
    QObject::connect(m_stateSendRMIdentAndRegister, &QAbstractState::entered, this, &MockPcbServer::doIdentAndRegister);
}

MockPcbServer::~MockPcbServer()
{
    if (m_pInitializationMachine) delete m_pInitializationMachine;
    if (m_pRMConnection) delete m_pRMConnection;
}

void MockPcbServer::setXmlSettings(XmlSettingsList xmlSettings)
{
    m_xmlSettings = xmlSettings;
    doConfiguration();
}

void MockPcbServer::setResources(ResourcesList resources)
{
    m_resources = resources;
}

Zera::XMLConfig::cReader *MockPcbServer::getConfigReader()
{
    return &m_xmlConfigReader;
}

ServerParams MockPcbServer::createParams(QString serviceName, QString sysrootDir)
{
    QString configPath;
    if(serviceName == "com5003d")
        configPath =  sysrootDir + QStringLiteral(CONFIG_TARGET_DIR_COM5003D);
    else if(serviceName == "mt310s2d")
        configPath =  sysrootDir + QStringLiteral(CONFIG_TARGET_DIR_MT310S2D);
    else if(serviceName == "sec1000d")
        configPath =  sysrootDir + QStringLiteral(CONFIG_TARGET_DIR_SEC1000D);
    else if(serviceName == "zdsp1d") // ooh that is far from mockable yet...
        configPath =  sysrootDir + QStringLiteral(CONFIG_TARGET_DIR_ZDSP1D);
    else
        qFatal("Cannot mock service: %s", qPrintable(serviceName));
    ServerParams params {
        QStringLiteral("Mock") + serviceName,
        "V42.0",
        QDir::cleanPath(configPath + "/" + serviceName + ".xsd"),
        QDir::cleanPath(configPath + "/" + serviceName + ".xml")
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
        for (const auto &setting : qAsConst(m_xmlSettings))
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
                    setting, &XMLSettings::configXMLInfo);
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
    for(const auto &resource : qAsConst(m_resources)) {
        scpiConnectionList.append(resource);
        resourceList.append(resource);
    }
    initSCPIConnections();

    m_myServer->startServer(m_ethSettings.getPort(EthSettings::protobufserver)); // and can start the server now
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
