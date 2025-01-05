#ifndef ZDSP1D_H
#define ZDSP1D_H

#include "abstractfactorydevicenodedsp.h"
#include "dspvardevicenodeinout.h"
#include "resourceregistertransaction.h"
#include "rmconnection.h"
#include "dspsettings.h"
#include "scpiconnection.h"
#include "settingscontainer.h"
#include <timertemplateqt.h>
#include <logstatisticsasyncint.h>
#include <scpi.h>
#include <vtcp_server.h>
#include <xiqnetwrapper.h>
#include <abstracttcpnetworkfactory.h>
#include <QStringList>
#include <QSocketNotifier>
#include <QByteArray>
#include <QTimer>
#include <QHash>
#include <QVector>
#include <QStateMachine>
#include <QTcpServer>
#include <QDomDocument>

class cZDSP1Client;

typedef QVector<float> tDspMemArray;


class ZDspServer: public ScpiConnection
{
    Q_OBJECT
public:
    ZDspServer(SettingsContainerPtr settings,
               AbstractFactoryDeviceNodeDspPtr deviceNodeFactory,
               VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory);
    virtual ~ZDspServer();
    QString getServerVersion();
    QString getDspDeviceNode();

    static const ServerParams defaultParams;
signals:
    void sigServerIsSetUp();
    void abortInit();

private slots:
    void sendProtoAnswer(cProtonetCommand *protoCmd);
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
    void initSCPIConnection(QString leadingNodes) override;
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;

    friend class TestZdsp1dForVarAccess;
    cZDSP1Client* AddClient(VeinTcp::TcpPeer *netClient);
    void addClientToHash(QByteArray clientId, VeinTcp::TcpPeer *peer);
    void DelClients(VeinTcp::TcpPeer *netClient);
    void DelClient(QByteArray clientId);
    cZDSP1Client* AddSCPIClient();
    void DelSCPIClient();

    QString handleScpiInterfaceRead(const QString &scpiInput);
    void outputDspRunState();
    void outputAndResetTransactionsLogs();
    void openTelnetScpi();

    AbstractFactoryDeviceNodeDspPtr m_deviceNodeFactory;
    DspVarDeviceNodeInOut m_dspInOut;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    SettingsContainerPtr m_settings;
    VeinTcp::TcpServer* m_protoBufServer = nullptr;
    XiQNetWrapper m_protobufWrapper;
    quint16 m_nSocketIdentifier = 0; // we will use this instead of real sockets, because protobuf extension clientId
    QTcpServer* m_telnetServer = nullptr;
    QTcpSocket* m_telnetSocket = nullptr;

    cDSPSettings* m_pDspSettings = nullptr;
    quint8 m_nerror;
    uchar ActivatedCmdList;
    QByteArray m_rawCyclicCmdMem; // unsere dsp programm listen
    QByteArray m_rawInterruptCmdMem;
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

    QString sendCommand2Dsp(QString); // indirekt für system modell
    QString getLcaAndDspVersion();
    QString getSamplingSystemSetup(cZDSP1Client *client);
    QString getDspCommandStat();
    QString setDspCommandStat(QString scpiParam);
    QString startTriggerIntListHKSK(QString scpiParam, int socket);

    // die routinen für das status modell
    QString getDeviceStatus();
    QString getDspStatus();

    // die routinen für das measure modell
    QString loadCmdList(cZDSP1Client *client);
    QString unloadCmdList(cZDSP1Client *client);

    bool BuildDSProgram(QString& errs);
    bool LoadDSProgram();
    bool setDspType();
    int readMagicId();
    bool Test4HWPresent();
    bool Test4DspRunning();
    cZDSP1Client* GetClient(int s);
    cZDSP1Client* GetClient(VeinTcp::TcpPeer* peer);
    void executeCommandProto(VeinTcp::TcpPeer* peer, std::shared_ptr<google::protobuf::Message> cmd);
    QString m_sDspBootPath;
    Zera::XMLConfig::cReader* myXMLConfigReader;

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
