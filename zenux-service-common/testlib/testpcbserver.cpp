#include "testpcbserver.h"
#include "mockserverparamgenerator.h"
#include <QFinalState>
#include <QDir>
#include <memory>

TestPcbServer::TestPcbServer(const QString &serviceNameForAlternateDevice,
                             const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory) :
    PCBServer(std::make_unique<SettingsContainer>(MockServerParamGenerator::createParams(serviceNameForAlternateDevice)),
              tcpNetworkFactory)
{
    m_pInitializationMachine = new QStateMachine(this);

    QState* stateCONF = new QState();
    QFinalState* stateFINISH = new QFinalState();

    QState* statesetupServer = new QState(stateCONF);
    stateCONF->setInitialState(statesetupServer);
    
    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);
    
    QObject::connect(statesetupServer, &QAbstractState::entered, this, &TestPcbServer::doSetupServer);
}

TestPcbServer::~TestPcbServer()
{
    if (m_pInitializationMachine) delete m_pInitializationMachine;
}

void TestPcbServer::setScpiConnections(const ScpiConnectionList &scpiConnections)
{
    m_scpiConnecttionsAddedFromExtern = scpiConnections;
}

Zera::XMLConfig::cReader *TestPcbServer::getConfigReader()
{
    return &m_xmlConfigReader;
}

void TestPcbServer::start()
{
    m_pInitializationMachine->start();
}

void TestPcbServer::doSetupServer()
{
    connectProtoConnectionSignals();
    m_scpiConnectionList.append(this); // the server itself has some commands
    for(const auto &scpiConnection : qAsConst(m_scpiConnecttionsAddedFromExtern))
        m_scpiConnectionList.append(scpiConnection);
    initSCPIConnections();

    EthSettingsPtr ethSettings = m_settings->getEthSettings();
    m_protoBufServer.startServer(ethSettings->getPort(EthSettings::protobufserver));

    emit sigServerIsSetUp();
}
