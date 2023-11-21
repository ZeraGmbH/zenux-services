#ifndef MOCKPCBSERVER_H
#define MOCKPCBSERVER_H

#include "pcbserver.h"
#include <xmlsettings.h>
#include <QStateMachine>
#include <memory.h>

typedef QList<XMLSettings*> XmlSettingsList;
typedef QList<cResource*> ResourcesList;

class MockPcbServer : public cPCBServer
{
    Q_OBJECT
public:
    MockPcbServer(QString serviceName);
    ~MockPcbServer();
    Zera::XMLConfig::cReader *getConfigReader();
    void setXmlSettings(XmlSettingsList xmlSettings);
    void setResources(ResourcesList resources);
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
    static ServerParams createParams(QString serviceName);
    XmlSettingsList m_xmlSettings;
    ResourcesList m_resources;
    int m_resourcesToConnect = 0;
    bool m_resourcesRegistered = false;
    QStateMachine* m_pInitializationMachine = nullptr;
    QState* m_stateconnect2RM = nullptr;
    QState* m_stateSendRMIdentAndRegister = nullptr;
    RMConnection* m_pRMConnection = nullptr;
};

#endif // MOCKPCBSERVER_H
