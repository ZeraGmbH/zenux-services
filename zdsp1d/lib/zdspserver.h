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

class ZdspClient;

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

    // Test analysis
    struct TClientCounts {
        int m_listCount = 0;
        int m_hashCount = 0;
        int m_interruptHashCount = 0;
    };
    TClientCounts getClientCounts() const;

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
    ZdspClient* AddClient(VeinTcp::TcpPeer *netClient, const QByteArray &clientId);
    void addClientToHash(const QByteArray &clientId, VeinTcp::TcpPeer *peer);
    void DelClients(VeinTcp::TcpPeer *netClient);
    void DelClient(QByteArray clientId);
    ZdspClient* AddSCPIClient();
    void DelSCPIClient();
    bool isClientStillThereAndActive(ZdspClient *client) const;

    QString handleScpiInterfaceRead(const QString &scpiInput);
    void outputDspRunState();
    void outputAndResetTransactionsLogs();
    void openTelnetScpi();

    Zera::XMLConfig::cReader m_xmlConfigReader;
    cDSPSettings m_dspSettings;
    SettingsContainerPtr m_settings;
    AbstractFactoryDeviceNodeDspPtr m_deviceNodeFactory;
    DspVarDeviceNodeInOut m_dspInOut;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    VeinTcp::TcpServer m_protoBufServer;
    XiQNetWrapper m_protobufWrapper;
    quint16 m_nSocketIdentifier = 0; // we will use this instead of real sockets, because protobuf extension clientId
    QTcpServer* m_telnetServer = nullptr;
    QTcpSocket* m_telnetSocket = nullptr;

    uchar m_currentCmdListSelector = 0;
    QByteArray m_rawCyclicCmdMem; // unsere dsp programm listen
    QByteArray m_rawInterruptCmdMem;
    QSocketNotifier* m_pNotifier = nullptr;

    QHash<QByteArray, ZdspClient*> m_zdspdClientHash;
    QHash<ZdspClient*, QByteArray> m_clientIDHash; // liste der clientID's für die dspclients die über protobuf erzeugt wurden
    ZdspClient* m_pSCPIClient = nullptr;
    QList<ZdspClient*> m_clientList;

    ulong UserWorkSpaceGlobalSegmentAdr;
    LogStatisticsAsyncInt m_dspInterruptLogStatistics;

    bool resetDsp();
    bool bootDsp();
    bool setSamplingSystem();

    QString sendCommand2Dsp(const QString &dspCmdParLine);
    QString getLcaAndDspVersion();
    QString getSamplingSystemSetup();
    QString getDspCommandStat();
    QString setDspCommandStat(const QString &scpiParam);
    QString runDspTest(const QString &scpiParam);
    QString startTriggerIntListHKSK(const QString &scpiParam, int socket);

    // die routinen für das status modell
    QString getDeviceStatus();
    QString getDspStatus();

    // die routinen für das measure modell
    QString loadCmdList(ZdspClient *client);
    QString unloadCmdList(ZdspClient *client);
    QString unloadCmdListAllClients();

    bool compileCmdListsForAllClientsToRawStream(QString& errs);
    bool uploadCommandLists();
    void flipCommandListSelector();
    bool writeDspCmdListsToDevNode();
    bool setDspType();
    int readMagicId();
    bool Test4HWPresent();
    bool Test4DspRunning();
    ZdspClient* GetClient(int s);
    void executeCommandProto(VeinTcp::TcpPeer* peer, std::shared_ptr<google::protobuf::Message> cmd);
    QString m_sDspBootPath;

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
