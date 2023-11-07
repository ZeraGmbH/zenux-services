#ifndef MOCKMT310S2D_H
#define MOCKMT310S2D_H

#include "pcbserver.h"
#include "foutgroupresourceandinterface.h"
#include <QStateMachine>

class MockMt310s2d : public cPCBServer
{
    Q_OBJECT
public:
    MockMt310s2d();
    ~MockMt310s2d();
    void start();
    bool areAllResourcesConnected();
signals:
    void serverSetup();

private slots:
    void doConfiguration();
    void doSetupServer();
    void doConnect2RM();
    void doIdentAndRegister();
    void onSetupComplete();
    void onResourceRegisterRdy();
private:
    int m_resourcesToConnect = 0;
    bool m_resourcesRegistered = false;
    QStateMachine* m_pInitializationMachine = nullptr;
    QState* m_stateconnect2RM = nullptr;
    QState* m_stateSendRMIdentAndRegister = nullptr;
    RMConnection* m_pRMConnection = nullptr;
    FOutSettings* m_foutSettings = nullptr;
    FOutGroupResourceAndInterface* m_foutInterface = nullptr;
};

#endif // MOCKMT310S2D_H
