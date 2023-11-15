#ifndef SEC1000D_H
#define SEC1000D_H

#include <QTimer>

#include "pcbserver.h"
#include "rmconnection.h"

class QStateMachine;
class QState;
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
    explicit cSEC1000dServer();
    ~cSEC1000dServer();

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
    int m_devFileDescriptor; // kerneltreiber wird nur 1x geöffnet und dann gehalten
    QStateMachine* m_pInitializationMachine;
    QState* m_stateconnect2RM;
    QState* m_stateconnect2RMError;
    QState* m_stateSendRMIdentAndRegister;
    quint8 m_nerror;
    int m_retryRMConnect;
    QTimer m_retryTimer;
    QString m_sSECDeviceNode;
    QList<SecChannel*> m_ECalculatorChannelList;
    int SECDevOpen();
    void SetFASync();

private slots:
    void SECIntHandler();
    void doConfiguration();
    void doSetupServer();
    void doCloseServer();
    void doConnect2RM();
    void connect2RMError();
    void doIdentAndRegister();
};


#endif
