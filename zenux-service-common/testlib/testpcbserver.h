#ifndef TESTPCBSERVER_H
#define TESTPCBSERVER_H

#include "pcbserver.h"
#include <xmlsettings.h>
#include <QStateMachine>

typedef QList<XMLSettings*> XmlSettingsList;
typedef QList<ScpiServerConnection*> ScpiConnectionList;

class TestPcbServer : public PCBServer
{
    Q_OBJECT
public:
    TestPcbServer(const QString &serviceNameForAlternateDevice,
                  const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory);
    ~TestPcbServer();
    Zera::XMLConfig::cReader *getConfigReader();
    void setScpiConnections(const ScpiConnectionList &scpiConnections);
    void start();
signals:
    void sigServerIsSetUp();

private slots:
    void doSetupServer();
private:
    ScpiConnectionList m_scpiConnecttionsAddedFromExtern;
    QStateMachine* m_pInitializationMachine = nullptr;
};

#endif // TESTPCBSERVER_H
