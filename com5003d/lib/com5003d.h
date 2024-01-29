#ifndef COM5003D_H
#define COM5003D_H

#include "abstractfactoryi2cctrl.h"
#include "pcbserver.h"
#include "rmconnection.h"
#include "fpgasettings.h"
#include "senseinterfacecommon.h"
#include "sensesettings.h"
#include "hkinsettings.h"
#include "scinsettings.h"
#include "finsettings.h"
#include "foutsettings.h"
#include "samplingsettings.h"
#include "abstractctrlheartbeatwait.h"
#include "settingsfordeviceserver.h"
#include <QTimer>

class QStateMachine;
class QState;
class cStatusInterface;
class Com5003SystemInterface;
class cSamplingInterface;
class FOutGroupResourceAndInterface;
class FInGroupResourceAndInterface;
class ScInGroupResourceAndInterface;
class HkInGroupResourceAndInterface;
class SystemInfo;


class cCOM5003dServer: public cPCBServer
{
    Q_OBJECT

public:
    explicit cCOM5003dServer(std::shared_ptr<SettingsForDeviceServer> settings,
                             AbstractFactoryI2cCtrlPtr ctrlFactory);
    ~cCOM5003dServer();
    QString getCtrlDeviceNode();
    static ServerParams defaultParams;

signals:
    void abortInit();
    void confStarting();
    void confFinished();
    void atmelProgrammed();
    void atmelRunning();
    void sigServerIsSetUp();

private slots:
    void doConfiguration();
    void programAtmelFlash();
    void doWait4Atmel();
    void doSetupServer();
    void doCloseServer();
    void doConnect2RM();
    void connect2RMError();
    void doIdentAndRegister();
private:
    void setupMicroControllerIo();

    std::shared_ptr<SettingsForDeviceServer> m_settings;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;

    SystemInfo* m_pSystemInfo = nullptr;

    cSenseSettings* m_pSenseSettings = nullptr;
    cStatusInterface* m_pStatusInterface = nullptr;
    HkInSettings* m_hkInSettings = nullptr;
    ScInSettings* m_pSCHeadSettings = nullptr;
    FInSettings* m_finSettings = nullptr;
    FOutSettings* m_foutSettings = nullptr;
    SamplingSettings* m_pSamplingSettings = nullptr;
    FPGASettings* m_fpgaSettings = nullptr;

    Com5003SystemInterface* m_pSystemInterface = nullptr;
    SenseInterfaceCommon* m_pSenseInterface = nullptr;
    cSamplingInterface* m_pSamplingInterface = nullptr;
    FOutGroupResourceAndInterface* m_foutInterface = nullptr;
    FInGroupResourceAndInterface* m_pFRQInputInterface = nullptr;
    ScInGroupResourceAndInterface* m_pSCHeadInterface = nullptr;
    HkInGroupResourceAndInterface* m_hkInInterface = nullptr;
    RMConnection* m_pRMConnection = nullptr;

    QStateMachine* m_pInitializationMachine = nullptr;
    QState* m_stateconnect2RM = nullptr;
    QState* m_stateconnect2RMError = nullptr;
    QState* m_stateSendRMIdentAndRegister = nullptr;
    AbstractCtrlHeartbeatWaitPtr m_ctrlHeartbeatWait;
    quint8 m_nerror;
    int m_nFPGAfd;
    int m_retryRMConnect;
    QTimer m_retryTimer;
};


#endif
