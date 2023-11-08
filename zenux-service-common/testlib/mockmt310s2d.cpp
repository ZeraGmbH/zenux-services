#include "mockmt310s2d.h"
#include "scpisingletonfactory.h"
#include <QFinalState>

static ServerParams params {
                           "MockMt310s2d",
                           "V1.0",
                           QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xsd",
                           QStringLiteral(CONFIG_SOURCES_MT310S2D) + "/" + "mt310s2d.xml"
};

MockMt310s2d::MockMt310s2d() :
    cPCBServer(params, ScpiSingletonFactory::getScpiObj())
{
    m_pInitializationMachine = new QStateMachine(this);

    QState* stateCONF = new QState(); // we start from here
    QFinalState* stateFINISH = new QFinalState(); // and here we finish

    QState* statexmlConfiguration = new QState(stateCONF); // we configure our server with xml file
    QState* statesetupServer = new QState(stateCONF); // we setup our server now
    m_stateconnect2RM = new QState(stateCONF); // we connect to resource manager
    m_stateSendRMIdentAndRegister = new QState(stateCONF); // we send ident. to rm and register our resources

    stateCONF->setInitialState(statexmlConfiguration);

    statexmlConfiguration->addTransition(&m_xmlConfigReader, &Zera::XMLConfig::cReader::finishedParsingXML, statesetupServer);
    statesetupServer->addTransition(this, &MockMt310s2d::sigServerIsSetUp, m_stateconnect2RM);

    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);

    QObject::connect(statexmlConfiguration, &QAbstractState::entered, this, &MockMt310s2d::doConfiguration);
    QObject::connect(statesetupServer, &QAbstractState::entered, this, &MockMt310s2d::doSetupServer);
    QObject::connect(m_stateconnect2RM, &QAbstractState::entered, this, &MockMt310s2d::doConnect2RM);
    QObject::connect(m_stateSendRMIdentAndRegister, &QAbstractState::entered, this, &MockMt310s2d::doIdentAndRegister);
}

MockMt310s2d::~MockMt310s2d()
{
    if (m_pInitializationMachine) delete m_pInitializationMachine;
    if (m_pRMConnection) delete m_pRMConnection;
    if (m_foutSettings) delete m_foutSettings;
    if (m_foutInterface) delete m_foutInterface;
}

void MockMt310s2d::start()
{
    m_pInitializationMachine->start();
}

bool MockMt310s2d::areAllResourcesConnected()
{
    return m_resourcesRegistered;
}

void MockMt310s2d::doConfiguration()
{
    if (m_xmlConfigReader.loadSchema(m_params.xsdFile)) {
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, &m_ethSettings, &EthSettings::configXMLInfo);
        m_foutSettings = new FOutSettings(&m_xmlConfigReader);
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_foutSettings, &FOutSettings::configXMLInfo);

        if (!m_xmlConfigReader.loadXMLFile(m_params.xmlFile))
            qFatal("Could not load xml config file");
    }
    else
        qFatal("Could not load xml schema");
}

void MockMt310s2d::doSetupServer()
{
    setupServer(); // here our scpi interface gets instanciated, we need this for further steps

    scpiConnectionList.append(this); // the server itself has some commands
    scpiConnectionList.append(m_foutInterface = new FOutGroupResourceAndInterface(getSCPIInterface(), m_foutSettings));

    resourceList.append(m_foutInterface);

    initSCPIConnections();

    myServer->startServer(m_ethSettings.getPort(EthSettings::protobufserver)); // and can start the server now
    if(m_ethSettings.isSCPIactive())
        m_pSCPIServer->listen(QHostAddress::AnyIPv4, m_ethSettings.getPort(EthSettings::scpiserver));

    // our resource mananager connection must be opened after configuration is done
    m_pRMConnection = new RMConnection(m_ethSettings.getRMIPadr(), m_ethSettings.getPort(EthSettings::resourcemanager));
    m_stateconnect2RM->addTransition(m_pRMConnection, &RMConnection::connected, m_stateSendRMIdentAndRegister);

    emit sigServerIsSetUp(); // so we enter state machine's next state
}

void MockMt310s2d::doConnect2RM()
{
    m_pRMConnection->connect2RM();
}

void MockMt310s2d::doIdentAndRegister()
{
    m_pRMConnection->SendIdent(getName());
    m_resourcesToConnect = resourceList.count();
    for (int i = 0; i < m_resourcesToConnect; i++) {
        cResource *res = resourceList.at(i);
        connect(m_pRMConnection, &RMConnection::rmAck, res, &cResource::resourceManagerAck);
        connect(res, &cResource::registerRdy, this, &MockMt310s2d::onResourceRegisterRdy);
        res->registerResource(m_pRMConnection, m_ethSettings.getPort(EthSettings::protobufserver));
    }
}

void MockMt310s2d::onResourceRegisterRdy()
{
    m_resourcesToConnect--;
    if(m_resourcesToConnect == 0)
        m_resourcesRegistered = true;
}
