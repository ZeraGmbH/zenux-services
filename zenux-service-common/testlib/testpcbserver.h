#ifndef TESTPCBSERVER_H
#define TESTPCBSERVER_H

#include "pcbserver.h"
#include <xmlsettings.h>
#include <QStateMachine>
#include <memory.h>

typedef QList<XMLSettings*> XmlSettingsList;
typedef QList<cResource*> ResourcesList;
typedef QList<ScpiConnection*> ScpiConnectionList;

class TestPcbServer : public PCBServer
{
    Q_OBJECT
public:
    TestPcbServer(QString serviceName, VeinTcp::AbstractTcpWorkerFactoryPtr tcpNetworkFactory);
    ~TestPcbServer();
    Zera::XMLConfig::cReader *getConfigReader();
    RMConnection* getRmConnection();
    void setXmlSettings(XmlSettingsList xmlSettings);
    // Note: add interface either to resources ot scpi connections - not both
    void setResources(ResourcesList resources);
    void setScpiConnections(ScpiConnectionList scpiConnections);
    void start();
signals:
    void sigServerIsSetUp();

private slots:
    void doConfiguration();
    void doSetupServer();
    void doConnect2RM();
    void doIdentAndRegister();
private:
    XmlSettingsList m_xmlSettings;
    ResourcesList m_resources;
    ScpiConnectionList m_scpiConnecttionsAddedFromExtern;
    int m_resourcesToConnect = 0;
    QStateMachine* m_pInitializationMachine = nullptr;
    QState* m_stateconnect2RM = nullptr;
    QState* m_stateSendRMIdentAndRegister = nullptr;
    RMConnection* m_pRMConnection = nullptr;
};

#endif // TESTPCBSERVER_H
