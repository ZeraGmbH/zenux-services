#ifndef MT310S2D_H
#define MT310S2D_H

#include "hotplugcontrollerinterface.h"
#include "mt310s2systeminfo.h"
#include "abstractfactorydevicenodepcb.h"
#include "pcbserver.h"
#include "rmconnection.h"
#include "senseinterfacecommon.h"
#include "settingscontainer.h"
#include "sensesettings.h"
#include "hkinsettings.h"
#include "scinsettings.h"
#include "finsettings.h"
#include "foutsettings.h"
#include "accumulatorsettings.h"
#include "accumulatorinterface.h"
#include "abstractctrlheartbeatwait.h"
#include "sourcecontrolsettings.h"
#include "sourcecontrolinterface.h"
#include <QStateMachine>
#include <QTimer>
#include <timertemplateqt.h>
#include <cputemp.h>

class ServiceStatusInterface;
class Mt310s2SystemInterface;
class cSamplingInterface;
class FOutGroupResourceAndInterface;
class FInGroupResourceAndInterface;
class ScInGroupResourceAndInterface;
class HkInGroupResourceAndInterface;
class QSocketNotifier;
class cClampInterface;

class cMT310S2dServer: public PCBServer
{
    Q_OBJECT
public:
    explicit cMT310S2dServer(SettingsContainerPtr settings,
                             AbstractFactoryI2cCtrlPtr ctrlFactory,
                             AbstractFactoryDeviceNodePcbPtr deviceNodeFactory,
                             VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                             AbstractChannelRangeFactoryPtr channelRangeFactory);
    ~cMT310S2dServer();
    QString getCtrlDeviceNode();
    QString getMsgDeviceNode();

    void startCpuTemperatureSendTimer();
    cSenseSettings* getSenseSettings();

    void MTIntHandler(int);
    HotPluggableControllerContainerPtr getHotPluggableControllerContainer();
    cClampInterface* getClampInterface() const;

signals:
    void abortInit();
    void confStarting();
    void confFinished();
    void atmelRunning();
    void sigServerIsSetUp();

private slots:
    void doConfiguration();
    void doWait4Atmel();
    void doSetupServer();
    void doCloseServer();
    void doConnect2RM();
    void connect2RMError();
    void doIdentAndRegister();
    void onResourceReady();
    void onCpuTemperatureSend();

private:
    void init();
    void earlySetup(AbstractChannelRangeFactoryPtr channelRangeFactory);
    void SetFASync();
    void enableClampInterrupt();
    void updateI2cDevicesConnected();
    void setupMicroControllerIo();

    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    AbstractFactoryDeviceNodePcbPtr m_deviceNodeFactory;

    Mt310s2SystemInfo* m_pSystemInfo = nullptr;

    cSenseSettings* m_pSenseSettings = nullptr;
    ServiceStatusInterface* m_pStatusInterface = nullptr;
    HkInSettings* m_hkInSettings = nullptr;
    ScInSettings* m_pSCHeadSettings = nullptr;
    FInSettings* m_finSettings = nullptr;
    FOutSettings* m_foutSettings = nullptr;
    // Mt310s2 specifics
    AccumulatorSettings* m_accumulatorSettings = nullptr;
    SourceControlSettings* m_sourceControlSettings = nullptr;
    HotPluggableControllerContainerPtr m_hotPluggableControllerContainer;

    Mt310s2SystemInterface* m_pSystemInterface = nullptr;
    HotplugControllerInterface* m_pHotControllerInterface = nullptr;
    SenseInterfaceCommon* m_pSenseInterface = nullptr;
    cSamplingInterface* m_pSamplingInterface = nullptr;
    FOutGroupResourceAndInterface* m_foutInterface = nullptr;
    FInGroupResourceAndInterface* m_pFRQInputInterface = nullptr;
    ScInGroupResourceAndInterface* m_pSCHeadInterface = nullptr;
    HkInGroupResourceAndInterface* m_hkInInterface = nullptr;
    RMConnection* m_pRMConnection = nullptr;
    cClampInterface* m_pClampInterface = nullptr;
    AccumulatorInterface* m_accumulatorInterface = nullptr;
    SourceControlInterface* m_sourceControlInterface = nullptr;
    int m_pendingResources = 0;

    quint16 m_atmelInterruptMask = 0;
    quint16 m_atmelSysCntrlInterruptMask = 0;

    QStateMachine* m_pInitializationMachine = nullptr;
    QState* m_stateconnect2RM = nullptr;
    QState* m_stateconnect2RMError = nullptr;
    QState* m_stateSendRMIdentAndRegister = nullptr;
    AbstractCtrlHeartbeatWaitPtr m_ctrlHeartbeatWait;
    int m_retryRMConnect;
    QTimer m_retryTimer;
    QSocketNotifier* m_pNotifier = nullptr;

    TimerTemplateQtPtr m_1sPeriodicTimer;
    I2cCtrlCpuTemperaturePtr m_i2cCtrlCpuTemperature;
    CpuTemp m_cpuTemperature;
};

#endif
