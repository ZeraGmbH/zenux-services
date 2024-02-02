#ifndef ZDSP1D_H
#define ZDSP1D_H

#include "fpgasettings.h"
#include "scpicmdinterpreter.h"
#include "pcbserver.h"
#include "scpicmds.h"
#include "rmconnection.h"
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
    ZDspServer(ServerParams params = defaultParams);
    virtual ~ZDspServer();
    QString getServerVersion();
    QString getDspDeviceNode();

    cZDSP1Client* AddClient(XiQNetPeer *m_pNetClient);
    void DelClients(XiQNetPeer *netClient);
    void DelClient(QByteArray clientId);
    cZDSP1Client* AddSCPIClient();
    void DelSCPIClient();

    QString SCPICmd( SCPICmdType, QChar*) override;
    QString SCPIQuery(SCPICmdType cmdEnum) override;

signals:
    void sigServerIsSetUp();
    void abortInit();

private:
    static ServerParams defaultParams;
    ServerParams m_params;
    ScpiCmdInterpreter* m_cmdInterpreter = nullptr;
    XiQNetServer* myProtonetServer; // the real server that does the communication job
    XiQNetWrapper m_ProtobufWrapper;
    quint16 m_nSocketIdentifier = 0; // we will use this instead of real sockets, because protobuf extension clientId
    QTcpServer* m_pSCPIServer = nullptr;
    QTcpSocket* m_pSCPISocket = nullptr;

    EthSettings* m_pETHSettings = nullptr;
    FPGASettings* m_fpgaSettings = nullptr;
    cDSPSettings* m_pDspSettings = nullptr;

    int m_actualSocket; // der aktive socket im Execute

    quint8 m_nerror;
    uchar ActivatedCmdList;
    QByteArray CmdMem; // unsere dsp programm listen
    QByteArray CmdIntMem;
    QSocketNotifier* m_pNotifier;

    QHash<QByteArray, cZDSP1Client*> m_zdspdClientHash;
    QHash<cZDSP1Client*, QByteArray> m_clientIDHash; // liste der clientID's für die dspclients die über protobuf erzeugt wurden
    cZDSP1Client* m_pSCPIClient;
    QList<cZDSP1Client*> m_clientList;

    ulong UserWorkSpaceGlobalSegmentAdr;

    bool resetDsp();
    bool bootDsp();
    bool setSamplingSystem();

    // die routinen für das system modell
    QString mCommand2Dsp(QString); // indirekt für system modell

    QString mTestDsp(QChar *);
    QString mResetDsp(QChar*);
    QString mBootDsp(QChar*);
    QString mGetPCBSerialNumber();
    QString getLcaAndDspVersion();
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
    QString m_sDspSerialNumber; // seriennummer der hardware
    QString m_sDspBootPath;
    Zera::XMLConfig::cReader* myXMLConfigReader;

    QString Answer;

    QStateMachine* m_pInitializationMachine;
    QState* m_stateconnect2RM;
    QState* m_stateconnect2RMError;
    QState* m_stateSendRMIdentAndRegister;
    RMConnection* m_pRMConnection = nullptr;

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
