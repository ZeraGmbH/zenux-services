#ifndef MT310S2D_H
#define MT310S2D_H

#include "pcbserver.h"
#include "rmconnection.h"
#include "debugsettings.h"
#include "fpgasettings.h"
#include "ctrlsettings.h"
#include "i2csettings.h"
#include "sensesettings.h"
#include "hkinsettings.h"
#include "scinsettings.h"
#include "finsettings.h"
#include "foutsettings.h"
#include "samplingsettings.h"
#include "accumulatorsettings.h"
#include "accumulatorinterface.h"
#include "mt310s2systeminfo.h"
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
    cCtrlSettings* m_pCtrlSettings;
    cStatusInterface* m_pStatusInterface;
    cI2CSettings* m_pI2CSettings;
    cSenseSettings* m_pSenseSettings;
    HkInSettings* m_hkInSettings;
    ScInSettings* m_pSCHeadSettings;
    FInSettings* m_finSettings;
    FOutSettings* m_foutSettings;
    SamplingSettings* m_pSamplingSettings;
    AccumulatorSettings* m_accumulatorSettings;
    cSystemInterface* m_pSystemInterface;
    cSenseInterface* m_pSenseInterface;
    cSamplingInterface* m_pSamplingInterface;
    FOutGroupResourceAndInterface* m_foutInterface;
    FInGroupResourceAndInterface* m_pFRQInputInterface;
    ScInGroupResourceAndInterface* m_pSCHeadInterface;
    HkInGroupResourceAndInterface* m_hkInInterface;
    Mt310s2SystemInfo* m_pSystemInfo;
    cAdjustment* m_pAdjHandler;
    RMConnection* m_pRMConnection;
    cClampInterface* m_pClampInterface;
    AccumulatorInterface* m_accumulatorInterface;

    int DevFileDescriptorCtrl; // kerneltreiber wird nur 1x geöffnet und dann gehalten
    int DevFileDescriptorMsg;

    quint16 m_atmelInterruptMask = 0;
    quint16 m_atmelSysCntrlInterruptMask = 0;

signals:
    void abortInit();
    void confStarting();
    void confFinished();
    void atmelRunning();
    void sigServerIsSetUp();

private:
    QStateMachine* m_pInitializationMachine;
    QState* m_stateconnect2RM;
    QState* m_stateconnect2RMError;
    QState* m_stateSendRMIdentAndRegister;
    cAtmelWatcher* m_pAtmelWatcher;
    quint8 m_nerror;
    int m_retryRMConnect;
    QTimer m_retryTimer;
    QSocketNotifier* m_pNotifier;
    QString m_sCtrlDeviceNode;
    QString m_sMessageDeviceNode;
    int CtrlDevOpen();
    int MessageDevOpen();
    void SetFASync();
    void enableClampInterrupt();
    void enableAccumulatorInterrupt();
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