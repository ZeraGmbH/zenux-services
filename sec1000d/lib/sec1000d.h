#ifndef SEC1000D_H
#define SEC1000D_H

#include "pcbserver.h"
#include "rmconnection.h"
#include "settingscontainer.h"
#include "abstractfactorydevicenodesec.h"
#include <QTimer>

class QStateMachine;
class QState;
class QSocketNotifier;
class SecCalculatorSettings;
class SecInputSettings;
class Sec1000StatusInterface;
class cSystemInterface;
class SecGroupResourceAndInterface;
class SecChannel;
class Sec1000SystemInfo;


class cSEC1000dServer: public PCBServer
{
    Q_OBJECT
public:
    explicit cSEC1000dServer(SettingsContainerPtr settings,
                             AbstractFactoryDeviceNodeSecPtr deviceNodeFactory,
                             VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory);
    ~cSEC1000dServer();
    QString getServerVersion();
    QString getSecDeviceNode();
    static const ServerParams defaultParams;

signals:
    void abortInit();
    void confStarting();
    void confFinished();
    void sigServerIsSetUp();

private slots:
    void SECIntHandler(int);
    void doConfiguration() override;
    void doSetupServer();
    void doCloseServer();
    void doConnect2RM();
    void connect2RMError();
    void doIdentAndRegister();
    void onResourceReady();
    void onProtobufDisconnect(VeinTcp::TcpPeer *peer) override;
private:
    void init();
    AbstractFactoryDeviceNodeSecPtr m_deviceNodeFactory;
    SecCalculatorSettings* m_pECalcSettings = nullptr;
    SecInputSettings* m_pInputSettings = nullptr;

    Sec1000StatusInterface* m_pStatusInterface = nullptr;
    cSystemInterface* m_pSystemInterface = nullptr;
    SecGroupResourceAndInterface* m_pECalculatorInterface = nullptr;
    Sec1000SystemInfo* m_pSystemInfo = nullptr;
    RMConnection* m_pRMConnection = nullptr;
    int m_pendingResources = 0;

    QStateMachine* m_pInitializationMachine = nullptr;
    QState* m_stateconnect2RM = nullptr;
    QState* m_stateconnect2RMError = nullptr;
    QState* m_stateSendRMIdentAndRegister = nullptr;
    quint8 m_nerror;
    int m_retryRMConnect;
    QTimer m_retryTimer;
    QList<SecChannel*> m_ECalculatorChannelList;
    QSocketNotifier* m_pNotifier = nullptr;
};


#endif
