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
class cSystemInterface;
class cSenseInterface;
class cSamplingInterface;
class FOutGroupResourceAndInterface;
class FInGroupResourceAndInterface;
class ScInGroupResourceAndInterface;
class HkInGroupResourceAndInterface;
class cSystemInfo;
class cAdjustment;


class cCOM5003dServer: public cPCBServer
{
    Q_OBJECT

public:
    explicit cCOM5003dServer(ServerParams params = defaultParams);
    ~cCOM5003dServer();

    cDebugSettings* m_pDebugSettings;
    FPGASettings* m_pFPGASettings;
    cStatusInterface* m_pStatusInterface;
    cI2CSettings* m_pI2CSettings;
    cSenseSettings* m_pSenseSettings;
    HkInSettings* m_hkInSettings;
    ScInSettings* m_pSCHeadSettings;
    FInSettings* m_finSettings;
    FOutSettings* m_foutSettings;
    SamplingSettings* m_pSamplingSettings;
    cSystemInterface* m_pSystemInterface;
    cSenseInterface* m_pSenseInterface;
    cSamplingInterface* m_pSamplingInterface;
    FOutGroupResourceAndInterface* m_foutInterface;
    FInGroupResourceAndInterface* m_pFRQInputInterface;
    ScInGroupResourceAndInterface* m_pSCHeadInterface;
    HkInGroupResourceAndInterface* m_hkInInterface;
    cSystemInfo* m_pSystemInfo;
    cAdjustment* m_pAdjHandler;
    RMConnection* m_pRMConnection;


signals:
    void abortInit();
    void confStarting();
    void confFinished();
    void atmelProgrammed();
    void atmelRunning();
    void sigServerIsSetUp();

private:
    static ServerParams defaultParams;
    ServerParams m_params;
    QStateMachine* m_pInitializationMachine;
    QState* m_stateconnect2RM;
    QState* m_stateconnect2RMError;
    QState* m_stateSendRMIdentAndRegister;
    AtmelWatcherInterfacePrt m_atmelWatcher;
    quint8 m_nerror;
    int m_nFPGAfd;
    int m_retryRMConnect;
    QTimer m_retryTimer;

private slots:
    void doConfiguration();
    void programAtmelFlash();
    void doWait4Atmel();
    void doSetupServer();
    void doCloseServer();
    void doConnect2RM();
    void connect2RMError();
    void doIdentAndRegister();
};


#endif
