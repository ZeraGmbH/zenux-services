#ifndef SEC1000D_H
#define SEC1000D_H

#include <QTimer>

#include "pcbserver.h"
#include "rmconnection.h"
#include "ethsettings.h"

class QStateMachine;
class QState;
class QSocketNotifier;
class cDebugSettings;
class FPGASettings;
class cECalculatorSettings;
class SecInputSettings;
class cStatusInterface;
class cSystemInterface;
class cECalculatorInterface;
class cECalculatorChannel;
class cSystemInfo;


class cSEC1000dServer: public cPCBServer
{
    Q_OBJECT

public:
    explicit cSEC1000dServer();
    ~cSEC1000dServer();

    cDebugSettings* m_pDebugSettings;
    FPGASettings* m_pFPGASettings;
    cECalculatorSettings* m_pECalcSettings;
    SecInputSettings* m_pInputSettings;

    cStatusInterface* m_pStatusInterface;
    cSystemInterface* m_pSystemInterface;
    cECalculatorInterface* m_pECalculatorInterface;
    cSystemInfo* m_pSystemInfo;
    RMConnection* m_pRMConnection;

    int DevFileDescriptor; // kerneltreiber wird nur 1x geöffnet und dann gehalten

signals:
    void abortInit();
    void confStarting();
    void confFinished();
    void sigServerIsSetUp();

private:
    QStateMachine* m_pInitializationMachine;
    QState* m_stateconnect2RM;
    QState* m_stateconnect2RMError;
    QState* m_stateSendRMIdentAndRegister;
    quint8 m_nerror;
    int m_retryRMConnect;
    QTimer m_retryTimer;
    QString m_sSECDeviceNode;
    QList<cECalculatorChannel*> m_ECalculatorChannelList;
    QSocketNotifier* m_pNotifier;
    int SECDevOpen();
    void SetFASync();

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
