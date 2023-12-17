#ifndef COM5003D_H
#define COM5003D_H

#include "pcbserver.h"
#include "rmconnection.h"
#include "debugsettings.h"
#include "fpgasettings.h"
#include "i2csettings.h"
#include "sensesettings.h"
#include "hkinsettings.h"
#include "scinsettings.h"
#include "finsettings.h"
#include "foutsettings.h"
#include "samplingsettings.h"
#include "atmelwatcherinterface.h"
#include <QTimer>

class QStateMachine;
class QState;
class cStatusInterface;
class Com5003SystemInterface;
class Com5003SenseInterface;
class cSamplingInterface;
class FOutGroupResourceAndInterface;
class FInGroupResourceAndInterface;
class ScInGroupResourceAndInterface;
class HkInGroupResourceAndInterface;
class cSystemInfo;
class Com5003Adjustment;


class cCOM5003dServer: public cPCBServer
{
    Q_OBJECT

public:
    explicit cCOM5003dServer(ServerParams params = defaultParams);
    ~cCOM5003dServer();

    cSystemInfo* m_pSystemInfo;
    Com5003Adjustment* m_pAdjHandler;

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
    static ServerParams defaultParams;
    ServerParams m_params;

    cDebugSettings* m_pDebugSettings;
    FPGASettings* m_fpgaCtrlSettings;
    cStatusInterface* m_pStatusInterface;
    cI2CSettings* m_pI2CSettings;
    cSenseSettings* m_pSenseSettings;
    HkInSettings* m_hkInSettings;
    ScInSettings* m_pSCHeadSettings;
    FInSettings* m_finSettings;
    FOutSettings* m_foutSettings;
    SamplingSettings* m_pSamplingSettings;
    Com5003SystemInterface* m_pSystemInterface;
    Com5003SenseInterface* m_pSenseInterface;
    cSamplingInterface* m_pSamplingInterface;
    FOutGroupResourceAndInterface* m_foutInterface;
    FInGroupResourceAndInterface* m_pFRQInputInterface;
    ScInGroupResourceAndInterface* m_pSCHeadInterface;
    HkInGroupResourceAndInterface* m_hkInInterface;
    RMConnection* m_pRMConnection;

    QStateMachine* m_pInitializationMachine;
    QState* m_stateconnect2RM;
    QState* m_stateconnect2RMError;
    QState* m_stateSendRMIdentAndRegister;
    AtmelWatcherInterfacePrt m_atmelWatcher;
    quint8 m_nerror;
    int m_nFPGAfd;
    int m_retryRMConnect;
    QTimer m_retryTimer;

};


#endif
