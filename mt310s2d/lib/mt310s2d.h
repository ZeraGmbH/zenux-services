#ifndef MT310S2D_H
#define MT310S2D_H

#include "pcbserver.h"
#include "rmconnection.h"
#include "settingsfordeviceserver.h"
#include "debugsettings.h"
#include "sensesettings.h"
#include "hkinsettings.h"
#include "scinsettings.h"
#include "finsettings.h"
#include "foutsettings.h"
#include "samplingsettings.h"
#include "accumulatorsettings.h"
#include "accumulatorinterface.h"
#include "mt310s2systeminfo.h"
#include "abstractctrlheartbeatwait.h"
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
class QSocketNotifier;
class cClampInterface;

class cMT310S2dServer: public cPCBServer
{
    Q_OBJECT
public:
    explicit cMT310S2dServer(std::shared_ptr<SettingsForDeviceServer> settings,
                             FactoryControllerAbstractPtr ctrlFactory);
    ~cMT310S2dServer();
    QString getCtrlDeviceNode();
    QString getMsgDeviceNode();
    static const ServerParams defaultParams;
public slots:

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
    void updateI2cDevicesConnected();
    void setupMicroControllerIo();

    std::shared_ptr<SettingsForDeviceServer> m_settings;
    FactoryControllerAbstractPtr m_ctrlFactory;

    Mt310s2SystemInfo* m_pSystemInfo = nullptr;

    cDebugSettings* m_pDebugSettings = nullptr;
    cSenseSettings* m_pSenseSettings = nullptr;
    cStatusInterface* m_pStatusInterface = nullptr;
    HkInSettings* m_hkInSettings = nullptr;
    ScInSettings* m_pSCHeadSettings = nullptr;
    FInSettings* m_finSettings = nullptr;
    FOutSettings* m_foutSettings = nullptr;
    SamplingSettings* m_pSamplingSettings = nullptr;
    // Mt310s2 specifics
    AccumulatorSettings* m_accumulatorSettings = nullptr;

    Mt310s2SystemInterface* m_pSystemInterface = nullptr;
    Mt310s2SenseInterface* m_pSenseInterface = nullptr;
    cSamplingInterface* m_pSamplingInterface = nullptr;
    FOutGroupResourceAndInterface* m_foutInterface = nullptr;
    FInGroupResourceAndInterface* m_pFRQInputInterface = nullptr;
    ScInGroupResourceAndInterface* m_pSCHeadInterface = nullptr;
    HkInGroupResourceAndInterface* m_hkInInterface = nullptr;
    RMConnection* m_pRMConnection = nullptr;
    cClampInterface* m_pClampInterface = nullptr;
    AccumulatorInterface* m_accumulatorInterface = nullptr;

    quint16 m_atmelInterruptMask = 0;
    quint16 m_atmelSysCntrlInterruptMask = 0;

    QStateMachine* m_pInitializationMachine = nullptr;
    QState* m_stateconnect2RM = nullptr;
    QState* m_stateconnect2RMError = nullptr;
    QState* m_stateSendRMIdentAndRegister = nullptr;
    AbstractCtrlHeartbeatWaitPtr m_ctrlHeartbeatWait;
    quint8 m_nerror;
    int m_retryRMConnect;
    QTimer m_retryTimer;
    QSocketNotifier* m_pNotifier = nullptr;
};


#endif
