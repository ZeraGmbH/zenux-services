#ifndef SEC1000D_H
#define SEC1000D_H

#include "pcbserver.h"
#include "rmconnection.h"
#include <QTimer>

class QStateMachine;
class QState;
class QSocketNotifier;
class cDebugSettings;
class FPGASettings;
class SecCalculatorSettings;
class SecInputSettings;
class cStatusInterface;
class cSystemInterface;
class SecGroupResourceAndInterface;
class SecChannel;
class cSystemInfo;


class cSEC1000dServer: public cPCBServer
{
    Q_OBJECT

public:
    explicit cSEC1000dServer(ServerParams params = defaultParams);
    ~cSEC1000dServer();
    QString getServerVersion();

    cDebugSettings* m_pDebugSettings;
    FPGASettings* m_pFPGASettings;
    SecCalculatorSettings* m_pECalcSettings;
    SecInputSettings* m_pInputSettings;

    cStatusInterface* m_pStatusInterface;
    cSystemInterface* m_pSystemInterface;
    SecGroupResourceAndInterface* m_pECalculatorInterface;
    cSystemInfo* m_pSystemInfo;
    RMConnection* m_pRMConnection;

signals:
    void abortInit();
    void confStarting();
    void confFinished();
    void sigServerIsSetUp();

private:
    static ServerParams defaultParams;
    ServerParams m_params;
    QStateMachine* m_pInitializationMachine;
    QState* m_stateconnect2RM;
    QState* m_stateconnect2RMError;
    QState* m_stateSendRMIdentAndRegister;
    quint8 m_nerror;
    int m_retryRMConnect;
    QTimer m_retryTimer;
    QList<SecChannel*> m_ECalculatorChannelList;
    QSocketNotifier* m_pNotifier;

private slots:
    void SECIntHandler(int);
    void doConfiguration();
    void doSetupServer();
    void doCloseServer();
    void doConnect2RM();
    void connect2RMError();
    void doIdentAndRegister();
};


#endif
