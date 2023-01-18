#ifndef MT310S2D_H
#define MT310S2D_H

#include "pcbserver.h"
#include "rmconnection.h"
#include "debugsettings.h"
#include "fpgasettings.h"
#include <QTimer>

class QStateMachine;
class QState;
class cStatusInterface;
class cSystemInterface;
class cSenseInterface;
class cSamplingInterface;
class FOutGroupResourceAndInterface;
class FInGroupResourceAndInterface;
class ScInGroupResourceAndInterface;
class HkInGroupResourceAndInterface;
class cSystemInfo;
class cAdjustment;
class cAtmelWatcher;
class QSocketNotifier;
class cClampInterface;

class cMT310S2dServer: public cPCBServer
{
    Q_OBJECT

public:
    explicit cMT310S2dServer();
    ~cMT310S2dServer();

    cDebugSettings* m_pDebugSettings;
    FPGASettings* m_pFPGASettings;
    cStatusInterface* m_pStatusInterface;
    cSystemInterface* m_pSystemInterface;
    cSenseInterface* m_pSenseInterface;
    cSamplingInterface* m_pSamplingInterface;
    FOutGroupResourceAndInterface* m_pSourceInterface;
    FInGroupResourceAndInterface* m_pFRQInputInterface;
    ScInGroupResourceAndInterface* m_pSCHeadInterface;
    HkInGroupResourceAndInterface* m_hkInInterface;
    cSystemInfo* m_pSystemInfo;
    cAdjustment* m_pAdjHandler;
    RMConnection* m_pRMConnection;
    cClampInterface* m_pClampInterface;

    int DevFileDescriptorCtrl; // kerneltreiber wird nur 1x geöffnet und dann gehalten
    int DevFileDescriptorMsg;

    quint16 m_atmelInterruptMask = 0;

signals:
    void abortInit();
    void confStarting();
    void confFinished();
    void atmelRunning();
    void serverSetup();

private:
    QStateMachine* m_pInitializationMachine;
    QState* stateconnect2RM;
    QState* stateconnect2RMError;
    QState* stateSendRMIdentandRegister;
    cAtmelWatcher* m_pAtmelWatcher;
    quint8 m_nerror;
    int m_nRetryRMConnect;
    QTimer m_retryTimer;
    QSocketNotifier* m_pNotifier;
    QString m_sCtrlDeviceNode;
    QString m_sMessageDeviceNode;
    int CtrlDevOpen();
    int MessageDevOpen();
    void SetFASync();
    void enableClampInterrupt();
    void updateI2cDevicesConnected();

private slots:
    void MTIntHandler(int);
    void doConfiguration();
    void doWait4Atmel();
    void doSetupServer();
    void doCloseServer();
    void doConnect2RM();
    void connect2RMError();
    void doIdentAndRegister();
};


#endif
