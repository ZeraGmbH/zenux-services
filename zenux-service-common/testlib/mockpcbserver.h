#ifndef MOCKPCBSERVER_H
#define MOCKPCBSERVER_H

#include "pcbserver.h"
#include <xmlsettings.h>
#include <QStateMachine>
#include <memory.h>

typedef std::shared_ptr<XMLSettings> XMLSettingsPtr;
typedef std::shared_ptr<cResource> ResourcePtr;
struct TResourceWithSettings
{
    XMLSettingsPtr settings;
    ResourcePtr resource;
};

class MockPcbServer : public cPCBServer
{
    Q_OBJECT
public:
    MockPcbServer(QString serviceName, QList<TResourceWithSettings> resourceWithSettings);
    ~MockPcbServer();
    static ServerParams createParams(QString serviceName);
    void start();
    bool areAllResourcesConnected();
signals:
    void sigServerIsSetUp();

private slots:
    void doConfiguration();
    void doSetupServer();
    void doConnect2RM();
    void doIdentAndRegister();
    void onResourceRegisterRdy();
private:
    QList<TResourceWithSettings> m_resourceWithSettings;
    int m_resourcesToConnect = 0;
    bool m_resourcesRegistered = false;
    QStateMachine* m_pInitializationMachine = nullptr;
    QState* m_stateconnect2RM = nullptr;
    QState* m_stateSendRMIdentAndRegister = nullptr;
    RMConnection* m_pRMConnection = nullptr;
};

#endif // MOCKPCBSERVER_H
