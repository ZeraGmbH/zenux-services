#ifndef ZDSP1D_H
#define ZDSP1D_H

#include "abstractfactorydevicenodedsp.h"
#include "resourceregistertransaction.h"
#include "scpicmdinterpreter.h"
#include "scpicmds.h"
#include "rmconnection.h"
#include "dspsettings.h"
#include "scpiconnection.h"
#include "settingscontainer.h"
#include <timertemplateqt.h>
#include <logstatisticsasyncint.h>
#include <scpi.h>
#include <vtcp_server.h>
#include <xiqnetwrapper.h>
#include <abstracttcpworkerfactory.h>
#include <QStringList>
#include <QSocketNotifier>
#include <QByteArray>
#include <QTimer>
#include <QHash>
#include <QVector>
#include <QStateMachine>
#include <QTcpServer>

class cZDSP1Client;

typedef QVector<float> tDspMemArray;


class ZDspServer: public ScpiConnection, public cbIFace
{
    Q_OBJECT
public:
    ZDspServer(SettingsContainerPtr settings,
               AbstractFactoryDeviceNodeDspPtr deviceNodeFactory,
               VeinTcp::AbstractTcpWorkerFactoryPtr tcpNetworkFactory);
    virtual ~ZDspServer();
    void initSCPIConnection(QString leadingNodes) override;
    QString getServerVersion();
    QString getDspDeviceNode();

    cZDSP1Client* AddClient(VeinTcp::TcpPeer *netClient);
    void DelClients(VeinTcp::TcpPeer *netClient);
    void DelClient(QByteArray clientId);
    cZDSP1Client* AddSCPIClient();
    void DelSCPIClient();

    QString SCPICmd( SCPICmdType, QChar*) override;
    QString SCPIQuery(SCPICmdType cmdEnum) override;

    static const ServerParams defaultParams;
signals:
    void sigServerIsSetUp();
    void abortInit();

private slots:
    void onProtobufClientConnected(VeinTcp::TcpPeer* newClient);
    void onProtobufDataReceived(VeinTcp::TcpPeer *peer, QByteArray message);
    void onProtobufDisconnect(VeinTcp::TcpPeer *peer);

    void onTelnetClientConnected();
    void onTelnetDataReceived();
    void onTelnetDisconnect();

    void DspIntHandler(int);
    void doConfiguration();
    void doSetupServer();
    void doCloseServer();
    void doConnect2RM();
    void connect2RMError();
    void doIdentAndRegister();
    void onResourceReady();

    void outputLogs();
private:
    void init();
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
    void outputDspRunState();
    void outputAndResetTransactionsLogs();
    void openTelnetScpi();

    AbstractFactoryDeviceNodeDspPtr m_deviceNodeFactory;
    VeinTcp::AbstractTcpWorkerFactoryPtr m_tcpNetworkFactory;
    SettingsContainerPtr m_settings;
    ScpiCmdInterpreter* m_cmdInterpreter = nullptr;
    VeinTcp::TcpServer* m_protoBufServer = nullptr;
    XiQNetWrapper m_protobufWrapper;
    quint16 m_nSocketIdentifier = 0; // we will use this instead of real sockets, because protobuf extension clientId
    QTcpServer* m_telnetServer = nullptr;
    QTcpSocket* m_telnetSocket = nullptr;

    cDSPSettings* m_pDspSettings = nullptr;

    int m_actualSocket; // der aktive socket im Execute

    quint8 m_nerror;
    uchar ActivatedCmdList;
    QByteArray CmdMem; // unsere dsp programm listen
    QByteArray CmdIntMem;
    QSocketNotifier* m_pNotifier = nullptr;

    QHash<QByteArray, cZDSP1Client*> m_zdspdClientHash;
    QHash<cZDSP1Client*, QByteArray> m_clientIDHash; // liste der clientID's für die dspclients die über protobuf erzeugt wurden
    cZDSP1Client* m_pSCPIClient = nullptr;
    QList<cZDSP1Client*> m_clientList;

    ulong UserWorkSpaceGlobalSegmentAdr;
    LogStatisticsAsyncInt m_dspInterruptLogStatistics;

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
    cZDSP1Client* GetClient(VeinTcp::TcpPeer* peer);
    void executeCommandProto(VeinTcp::TcpPeer* peer, std::shared_ptr<google::protobuf::Message> cmd);
    QString m_sDspSerialNumber; // seriennummer der hardware
    QString m_sDspBootPath;
    Zera::XMLConfig::cReader* myXMLConfigReader;

    QString Answer;

    QStateMachine* m_pInitializationMachine;
    QState* m_stateconnect2RM;
    QState* m_stateconnect2RMError;
    QState* m_stateSendRMIdentAndRegister;
    RMConnection* m_pRMConnection = nullptr;
    ResourceRegisterTransaction m_resourceRegister;

    int m_retryRMConnect;
    QTimer m_retryTimer;
    TimerTemplateQtPtr m_periodicLogTimer;
};

#endif
