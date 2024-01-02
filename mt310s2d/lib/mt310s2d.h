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
#include "atmelwatcherinterface.h"
#include <memory>
#include <QStateMachine>
#include <QTimer>

class cStatusInterface;
class Mt310s2SystemInterface;
class Mt310s2SenseInterface;
class cSamplingInterface;
class FOutGroupResourceAndInterface;
class FInGroupResourceAndInterface;
class ScInGroupResourceAndInterface;
class HkInGroupResourceAndInterface;
class Mt310s2Adjustment;
class QSocketNotifier;
class cClampInterface;

class cMT310S2dServer: public cPCBServer
{
    Q_OBJECT
public:
    explicit cMT310S2dServer(ServerParams params = defaultParams);
    ~cMT310S2dServer();

    Mt310s2SystemInfo* m_pSystemInfo;
    cSenseSettings* m_pSenseSettings;
    Mt310s2SenseInterface* m_pSenseInterface;

signals:
    void abortInit();
    void confStarting();
    void confFinished();
    void atmelRunning();
    void sigServerIsSetUp();

private slots:
    void MTIntHandler(int);
    void doConfiguration();
    void doWait4Atmel();
    void doSetupServer();
    void doCloseServer();
    void doConnect2RM();
    void connect2RMError();
    void doIdentAndRegister();
private:
    void SetFASync();
    void enableClampInterrupt();
    void enableAccumulatorInterrupt();
    void updateI2cDevicesConnected();

    static ServerParams defaultParams;
    ServerParams m_params;

    cDebugSettings* m_pDebugSettings;
    FPGASettings* m_fpgaMsgSettings;
    cCtrlSettings* m_fpgaCtrlSettings;
    cStatusInterface* m_pStatusInterface;
    cI2CSettings* m_pI2CSettings;
    HkInSettings* m_hkInSettings;
    ScInSettings* m_pSCHeadSettings;
    FInSettings* m_finSettings;
    FOutSettings* m_foutSettings;
    SamplingSettings* m_pSamplingSettings;
    AccumulatorSettings* m_accumulatorSettings;
    Mt310s2SystemInterface* m_pSystemInterface;
    cSamplingInterface* m_pSamplingInterface;
    FOutGroupResourceAndInterface* m_foutInterface;
    FInGroupResourceAndInterface* m_pFRQInputInterface;
    ScInGroupResourceAndInterface* m_pSCHeadInterface;
    HkInGroupResourceAndInterface* m_hkInInterface;
    Mt310s2Adjustment* m_pAdjHandler;
    RMConnection* m_pRMConnection;
    cClampInterface* m_pClampInterface;
    AccumulatorInterface* m_accumulatorInterface;
    std::shared_ptr<cATMELSysCtrl> m_systemController;

    quint16 m_atmelInterruptMask = 0;
    quint16 m_atmelSysCntrlInterruptMask = 0;

    QStateMachine* m_pInitializationMachine;
    QState* m_stateconnect2RM;
    QState* m_stateconnect2RMError;
    QState* m_stateSendRMIdentAndRegister;
    AtmelWatcherInterfacePtr m_atmelWatcher;
    quint8 m_nerror;
    int m_retryRMConnect;
    QTimer m_retryTimer;
    QSocketNotifier* m_pNotifier;
};


#endif
