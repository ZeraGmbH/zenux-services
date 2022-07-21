// header datei zdsp1d.h
// globale definitionen


#ifndef ZDSP1D_H
#define ZDSP1D_H

#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>
#include <QSocketNotifier>
#include <QByteArray>
#include <QTimer>
#include <QMap>
#include <QHash>
#include <QVector>
#include <xiqnetserver.h>
#include <xiqnetwrapper.h>

#include "dsp1scpi.h"
#include "zhserver.h"
#include "dsp.h"


typedef QVector<float> tDspMemArray;

class QByteArray;
class QStateMachine;
class QState;
class cZDSP1Server; // forward
class cDebugSettings;
class cETHSettings;
class cDSPSettings;
class cRMConnection;
class QTcpServer;
class QTcpSocket;

class cZDSP1Client
{
public:
    cZDSP1Client(){}
    cZDSP1Client(int socket, XiQNetPeer *netclient, cZDSP1Server *server);
    ~cZDSP1Client(){} //  allokierten speicher ggf. freigeben


    QString& SetRavList(QString&);
    QString& GetRavList();
    QString& SetCmdListDef(QString& );
    QString& GetCmdListDef();
    QString& SetCmdIntListDef(QString&);
    QString& GetCmdIntListDef();
    void SetEncryption(int);
    int GetEncryption();
    bool GenCmdLists(QString&, ulong, ulong); // baut die cmdlisten  für den dsp zusammen wenn fehler -> false
    cDspCmd GenDspCmd(QString&, bool*, ulong, ulong); // generiert ein dsp kommando aus einem string
    QString &readActValues(QString&); // liess die messergebnisse (liste)
    bool isActive();
    void SetActive(bool); // merkt sich in m_bActive ob diese liste aktiv ist
    ulong setStartAdr(ulong, ulong); // zum relokalisieren der userdaten
    QString &DspVarListRead(QString&); // lesen dsp daten ganze Liste
    bool DspVar(QString&,int&); // einen int (32bit) wert lesen
    bool DspVar(QString&,float&); // eine float wert lesen
    sDspVar* DspVarRead(QString&,QByteArray*); // lesen dsp variable;  name , länge stehen im parameter string; werte im anschluss im qbytearray
    const char* DspVarWriteRM(QString&); // dito schreiben mit rückmeldung
    bool DspVarWrite(QString&);  // schreiben  true wenn ok
    QList<cDspCmd>& GetDspCmdList(); // damit der server die komplette liste aller clients
    QList<cDspCmd>& GetDspIntCmdList(); // an den dsp übertragen kann
    int getSocket();
    cDspVarResolver DspVarResolver; // zum auflösen der variablen aus kommandos
    int sock; // socket für den die verbindung besteht
    XiQNetPeer* m_pNetClient; // our network client

private:
    void init(int socket, XiQNetPeer *netclient, cZDSP1Server* server);
    cZDSP1Server* myServer;
    bool m_bActive;
    bool GenCmdList(QString&, QList<cDspCmd>& ,QString&,ulong,ulong);
    bool syntaxCheck(QString&);

    int Encryption;
    char* qSEncryption(char*,int );
    QString sOutput;
    QString m_sCmdListDef; // kommando liste defintion
    QString m_sIntCmdListDef; // interrupt kommando  liste defintion

//    ulong m_nStartAdr; // die absolute adresse an der ein variablen "block" im dsp steht
//    int m_nlen; // länge des gesamten datenblocks (in float bzw. long)
    QList<cDspClientVar> m_DspVarList; // liste mit variablen beschreibungen
    QList<cDspCmd> m_DspCmdList; // liste mit dsp kommandos (periodisch)
    QList<cDspCmd>  m_DspIntCmdList; // liste mit dsp kommandos (interrupt)
    QVector<sDspVar> varArray; // array von sDspVar
    sMemSection msec; // eine memory section für den DspVarResolver für die variablen des clients

};

class cZDSP1Server: public QObject, public cZHServer, public cbIFace {
    Q_OBJECT
public:
    cZDSP1Server();
    virtual ~cZDSP1Server();
    virtual cZDSP1Client* AddClient(XiQNetPeer *m_pNetClient); // fügt einen client hinzu
    virtual void DelClient(XiQNetPeer *netClient); // entfernt einen client
    virtual void DelClient(QByteArray clientId);
    virtual cZDSP1Client* AddSCPIClient();
    virtual void DelSCPIClient();

    virtual QString SCPICmd( SCPICmdType, QChar*);
    virtual QString SCPIQuery( SCPICmdType);

    void SetFASync(); // async. benachrichtung einschalten

    int SetBootPath(const char*);
    int SetDeviceNode(char*);
    int SetDebugLevel(const char*);

    int DspDevRead(int, char*, int);
    int DspDevWrite(int, char*, int);
    int DspDevSeek(int,ulong);
    int DspDevOpen();

    int DevFileDescriptor; // kerneltreiber wird nur 1x geöffnet und dann gehalten
    int m_nDebugLevel;

    cDebugSettings* m_pDebugSettings;
    cETHSettings* m_pETHSettings;
    cDSPSettings* m_pDspSettings;

signals:
    void serverSetup();
    void sendAnswer(QByteArray);
    void abortInit();

private:
    XiQNetServer* myProtonetServer; // the real server that does the communication job
    XiQNetWrapper m_ProtobufWrapper;
    quint16 m_nSocketIdentifier; // we will use this instead of real sockets, because protobuf extension clientId
    QHash<QByteArray, cZDSP1Client*> m_zdspdClientHash;
    QHash<cZDSP1Client*, QByteArray> m_clientIDHash; // liste der clientID's für die dspclients die über protobuf erzeugt wurden
    QTcpServer* m_pSCPIServer;
    QTcpSocket* m_pSCPISocket;
    cZDSP1Client* m_pSCPIClient;

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
    QString mSetDspBootPath(QChar*);
    QString mGetDspBootPath();
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
    QString mSetRavList(QChar*);
    QString mGetRavList();
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
    QState* stateconnect2RM;
    QState* stateconnect2RMError;
    QState* stateSendRMIdentandRegister;
    cRMConnection* m_pRMConnection;

    int m_nRetryRMConnect;
    QTimer m_retryTimer;

private slots:
    virtual void establishNewConnection(XiQNetPeer* newClient);
    virtual void deleteConnection();
    virtual void executeCommand(std::shared_ptr<google::protobuf::Message> cmd);

    virtual void setSCPIConnection();
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
