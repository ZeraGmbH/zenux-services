#ifndef TESTPCBSERVER_H
#define TESTPCBSERVER_H

#include "pcbserver.h"
#include <xmlsettings.h>
#include <QStateMachine>
#include <memory.h>

typedef QList<XMLSettings*> XmlSettingsList;
typedef QList<ScpiServerConnection*> ScpiConnectionList;

class TestPcbServer : public PCBServer
{
    Q_OBJECT
public:
    TestPcbServer(QString serviceNameForAlternateDevice,
                  VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory);
    ~TestPcbServer();
    Zera::XMLConfig::cReader *getConfigReader();
    RMConnection* getRmConnection();
    void setScpiConnections(ScpiConnectionList scpiConnections);
    void start();
signals:
    void sigServerIsSetUp();

private slots:
    void doSetupServer();
    void doConnect2RM();
    void doIdentAndRegister();
private:
    ScpiConnectionList m_scpiConnecttionsAddedFromExtern;
    int m_resourcesToConnect = 0;
    QStateMachine* m_pInitializationMachine = nullptr;
    QState* m_stateconnect2RM = nullptr;
    QState* m_stateSendRMIdentAndRegister = nullptr;
    RMConnection* m_pRMConnection = nullptr;
};

#endif // TESTPCBSERVER_H
