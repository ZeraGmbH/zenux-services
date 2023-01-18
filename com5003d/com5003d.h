#ifndef COM5003D_H
#define COM5003D_H

#include "pcbserver.h"
#include "rmconnection.h"
#include "debugsettings.h"
#include "fpgasettings.h"
#include "i2csettings.h"
#include "sensesettings.h"
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


class cCOM5003dServer: public cPCBServer
{
    Q_OBJECT

public:
    explicit cCOM5003dServer();
    ~cCOM5003dServer();

    cDebugSettings* m_pDebugSettings;
    FPGASettings* m_pFPGASettings;
    cStatusInterface* m_pStatusInterface;
    cI2CSettings* m_pI2CSettings;
    cSenseSettings* m_pSenseSettings;
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


signals:
    void abortInit();
    void confStarting();
    void confFinished();
    void atmelProgrammed();
    void atmelRunning();
    void serverSetup();

private:
    QStateMachine* m_pInitializationMachine;
    QState* stateconnect2RM;
    QState* stateconnect2RMError;
    QState* stateSendRMIdentandRegister;
    cAtmelWatcher* m_pAtmelWatcher;
    quint8 m_nerror;
    int m_nFPGAfd;
    int m_nRetryRMConnect;
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
