// header datei com5003.h
// globale definitionen

#ifndef COM5003D_H
#define COM5003D_H

#include <QTimer>

#include "pcbserver.h"

class QStateMachine;
class QState;
class cStatusInterface;
class cSystemInterface;
class cSenseInterface;
class cSamplingInterface;
class cSourceInterface;
class cFRQInputInterface;
class cSCHeadInterface;
class cHKeyInterface;
class cSystemInfo;
class cAdjustment;
class cAtmelWatcher;
class cRMConnection;


class cCOM5003dServer: public cPCBServer
{
    Q_OBJECT

public:
    explicit cCOM5003dServer();
    ~cCOM5003dServer();

    cStatusInterface* m_pStatusInterface;
    cSystemInterface* m_pSystemInterface;
    cSenseInterface* m_pSenseInterface;
    cSamplingInterface* m_pSamplingInterface;
    cSourceInterface* m_pSourceInterface;
    cFRQInputInterface* m_pFRQInputInterface;
    cSCHeadInterface* m_pSCHeadInterface;
    cHKeyInterface* m_pHKeyInterface;
    cSystemInfo* m_pSystemInfo;
    cAdjustment* m_pAdjHandler;
    cRMConnection* m_pRMConnection;


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
