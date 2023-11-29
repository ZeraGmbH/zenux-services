#ifndef ZDSP1D_H
#define ZDSP1D_H

#include "cmdinterpret.h"
#include "dsp1scpi.h"
#include "rmconnection.h"
#include "debugsettings.h"
#include "ethsettings.h"
#include "dspsettings.h"
#include <xiqnetserver.h>
#include <xiqnetwrapper.h>
#include <QStringList>
#include <QSocketNotifier>
#include <QByteArray>
#include <QTimer>
#include <QHash>
#include <QVector>
#include <QStateMachine>

class cZDSP1Client;

typedef QVector<float> tDspMemArray;


class ZDspServer: public QObject, public cbIFace
{
    Q_OBJECT
public:
    ZDspServer();
    virtual ~ZDspServer();
    virtual cZDSP1Client* AddClient(XiQNetPeer *m_pNetClient); // fügt einen client hinzu
    virtual void DelClient(XiQNetPeer *netClient); // entfernt einen client
    virtual void DelClient(QByteArray clientId);
    virtual cZDSP1Client* AddSCPIClient();
    virtual void DelSCPIClient();

    virtual QString SCPICmd( SCPICmdType, QChar*);
    virtual QString SCPIQuery( SCPICmdType);

    void SetFASync(); // async. benachrichtung einschalten

    int DspDevOpen();
    int DspDevRead(char* buf, int len);
    int DspDevWrite(char* buf,int len);
    int DspDevSeek(ulong adr);

signals:
    void sigServerIsSetUp();
    void abortInit();

private:
    int m_devFileDescriptor; // kerneltreiber wird nur 1x geöffnet und dann gehalten
    cCmdInterpreter* m_cmdInterpreter = nullptr;
    XiQNetServer* myProtonetServer; // the real server that does the communication job
    XiQNetWrapper m_ProtobufWrapper;
    quint16 m_nSocketIdentifier = 0; // we will use this instead of real sockets, because protobuf extension clientId
    QHash<QByteArray, cZDSP1Client*> m_zdspdClientHash;
    QHash<cZDSP1Client*, QByteArray> m_clientIDHash; // liste der clientID's für die dspclients die über protobuf erzeugt wurden
    QTcpServer* m_pSCPIServer;
    QTcpSocket* m_pSCPISocket;
    cZDSP1Client* m_pSCPIClient;
    cDebugSettings* m_pDebugSettings = nullptr;
    EthSettings* m_pETHSettings = nullptr;
    cDSPSettings* m_pDspSettings = nullptr;
    int m_actualSocket; // der aktive socket im Execute

    quint8 m_nerror;
    uchar ActivatedCmdList;
    QByteArray CmdMem; // unsere dsp programm listen
    QByteArray CmdIntMem;
    QList<cZDSP1Client*> clientlist; // liste aller clients
    QSocketNotifier* m_pNotifier;

    ulong UserWorkSpaceGlobalSegmentAdr;

    bool resetDsp();
    bool bootDsp();
    bool setSamplingSystem();

    // die routinen für das system modell
    QString mCommand2Dsp(QString&); // indirekt für system modell

    QString mTestDsp(QChar *);
    QString mResetDsp(QChar*);
    QString mBootDsp(QChar*);
    QString mGetPCBSerialNumber();
    QString mGetDeviceVersion();
    QString mGetServerVersion();
    QString mSetSamplingSystem(QChar*);
    QString mGetSamplingSystem();
    QString mSetCommEncryption(QChar*);
    QString mGetCommEncryption();
    QString mSetEN61850DestAdr(QChar*);
    QString mGetEN61850DestAdr();
    QString mSetEN61850SourceAdr(QChar*);
    QString mGetEN61850SourceAdr();
    QString mSetEN61850EthTypeAppId(QChar*);
    QString mGetEN61850EthTypeAppId();
    QString mSetEN61850PriorityTagged(QChar*);
    QString mGetEN61850PriorityTagged();
    QString mSetEN61850EthSync(QChar*);
    QString mGetEN61850EthSync();
    QString mSetDspCommandStat(QChar*);
    QString mGetDspCommandStat();
    QString mSetEN61850DataCount(QChar*);
    QString mGetEN61850DataCount();
    QString mSetEN61850SyncLostCount(QChar*);
    QString mGetEN61850SyncLostCount();
    QString mTriggerIntListHKSK(QChar*);
    QString mTriggerIntListALL(QChar*);
    QString mResetMaxima(QChar*);

    // die routinen für das status modell

    QString mResetDeviceLoadMax();
    QString mGetDeviceLoadMax();
    QString mGetDeviceLoadAct();
    QString mGetDspStatus();
    QString mGetDeviceStatus();

    // die routinen für das measure modell

    QString mUnloadCmdList(QChar*);
    QString mLoadCmdList(QChar*);
    QString setRawActualValueList(QChar*);
    QString getRawActualValueList();
    QString mSetCmdIntList(QChar*);
    QString mGetCmdIntList();
    QString mSetCmdList(QChar*);
    QString mGetCmdList();
    QString mMeasure(QChar*);

    // die routinen für das memory modell

    QString mDspMemoryRead(QChar *);
    QString mDspMemoryWrite(QChar *);

    bool BuildDSProgram(QString& errs);
    bool LoadDSProgram();
    bool setDspType();
    int readMagicId();
    bool Test4HWPresent();
    bool Test4DspRunning();
    cZDSP1Client* GetClient(int s);
    cZDSP1Client* GetClient(XiQNetPeer* peer);
    QString m_sDspDeviceVersion; // version der hardware
    QString m_sDspSerialNumber; // seriennummer der hardware
    QString m_sDspDeviceNode; // für den zugriff zur hardware
    QString m_sDspBootPath;
    Zera::XMLConfig::cReader* myXMLConfigReader;

    QString Answer;

    QStateMachine* m_pInitializationMachine;
    QState* m_stateconnect2RM;
    QState* m_stateconnect2RMError;
    QState* m_stateSendRMIdentAndRegister;
    RMConnection* m_pRMConnection;

    int m_retryRMConnect;
    QTimer m_retryTimer;

private slots:
    virtual void onEstablishNewConnection(XiQNetPeer* newClient);
    virtual void deleteConnection();
    void onExecuteCommandProto(std::shared_ptr<google::protobuf::Message> cmd);

    void setSCPIConnection();
    virtual void SCPIInput();
    virtual void SCPIdisconnect();

    void DspIntHandler(int);
    void doConfiguration();
    void doSetupServer();
    void doCloseServer();
    void doConnect2RM();
    void connect2RMError();
    void doIdentAndRegister();
};

#endif
