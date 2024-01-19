#ifndef SEC1000D_H
#define SEC1000D_H

#include "pcbserver.h"
#include "rmconnection.h"
#include "fpgasettings.h"
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


class cSEC1000dServer: public cPCBServer
{
    Q_OBJECT

public:
    explicit cSEC1000dServer(ServerParams params = defaultParams);
    ~cSEC1000dServer();
    QString getServerVersion();
    QString getSecDeviceNode();

signals:
    void abortInit();
    void confStarting();
    void confFinished();
    void sigServerIsSetUp();

private slots:
    void SECIntHandler(int);
    void doConfiguration();
    void doSetupServer();
    void doCloseServer();
    void doConnect2RM();
    void connect2RMError();
    void doIdentAndRegister();
    void onPeerDisconnected();
private:
    static ServerParams defaultParams;
    ServerParams m_params;

    FPGASettings* m_pFPGASettings = nullptr;
    SecCalculatorSettings* m_pECalcSettings = nullptr;
    SecInputSettings* m_pInputSettings = nullptr;

    Sec1000StatusInterface* m_pStatusInterface = nullptr;
    cSystemInterface* m_pSystemInterface = nullptr;
    SecGroupResourceAndInterface* m_pECalculatorInterface = nullptr;
    Sec1000SystemInfo* m_pSystemInfo = nullptr;
    RMConnection* m_pRMConnection = nullptr;

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
