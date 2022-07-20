// header datei com5003.h
// globale definitionen

#ifndef SEC1000D_H
#define SEC1000D_H

#include <QTimer>

#include "pcbserver.h"

class QStateMachine;
class QState;
class QSocketNotifier;
class cDebugSettings;
class cFPGASettings;
class cETHSettings;
class cECalculatorSettings;
class cInputSettings;
class cStatusInterface;
class cSystemInterface;
class cECalculatorInterface;
class cECalculatorChannel;
class cSystemInfo;
class cRMConnection;


class cSEC1000dServer: public cPCBServer
{
    Q_OBJECT

public:
    explicit cSEC1000dServer();
    ~cSEC1000dServer();

    cDebugSettings* m_pDebugSettings;
    cFPGASettings* m_pFPGAsettings;
    cETHSettings* m_pETHSettings;
    cECalculatorSettings* m_pECalcSettings;
    cInputSettings* m_pInputSettings;

    cStatusInterface* m_pStatusInterface;
    cSystemInterface* m_pSystemInterface;
    cECalculatorInterface* m_pECalculatorInterface;
    cSystemInfo* m_pSystemInfo;
    cRMConnection* m_pRMConnection;

    int DevFileDescriptor; // kerneltreiber wird nur 1x geöffnet und dann gehalten
    int m_nDebugLevel;

signals:
    void abortInit();
    void confStarting();
    void confFinished();
    void serverSetup();

private:
    QStateMachine* m_pInitializationMachine;
    QState* stateconnect2RM;
    QState* stateconnect2RMError;
    QState* stateSendRMIdentandRegister;
    quint8 m_nerror;
    int m_nRetryRMConnect;
    QTimer m_retryTimer;
    QString m_sSECDeviceNode;
    QList<cECalculatorChannel*> m_ECalculatorChannelList;
    QSocketNotifier* m_pNotifier;
    int SECDevOpen();
    void SetFASync();

private slots:
    void SECIntHandler(int);
    void doConfiguration();
    void doSetupServer();
    void doCloseServer();
    void doConnect2RM();
    void connect2RMError();
    void doIdentAndRegister();
};


#endif
