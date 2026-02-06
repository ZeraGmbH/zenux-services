#ifndef ZDSP1D_H
#define ZDSP1D_H

#include "abstractfactorydevicenodedsp.h"
#include "consoleserver.h"
#include "dspvardevicenodeinout.h"
#include "resourceregistertransaction.h"
#include "rmconnection.h"
#include "dspsettings.h"
#include "zdspclientcontainer.h"
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
#include <QVector>
#include <QStateMachine>

typedef QVector<float> tDspMemArray;


class ZDspServer: public ScpiConnection
{
    Q_OBJECT
public:
    ZDspServer(SettingsContainerPtr settings,
               AbstractFactoryDeviceNodeDspPtr deviceNodeFactory,
               VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
               bool outputHealthLogs);
    virtual ~ZDspServer();
    QString getServerVersion();
    QString getDspDeviceNode();

    static const ServerParams defaultParams;
    ZdspClient *addClientForTest(); // TODO: remove
signals:
    void sigServerIsSetUp();
    void abortInit();

private slots:
    void sendProtoAnswer(ProtonetCommandPtr protoCmd);
    void onProtobufClientConnected(VeinTcp::TcpPeer* newClient);
    void onProtobufDataReceived(VeinTcp::TcpPeer *peer, const QByteArray &message);
    void onProtobufDisconnect(VeinTcp::TcpPeer *peer);
    void onTelnetReceived(const QString &input);

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
    void initSCPIConnection();
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;

    friend class TestZdsp1dForVarAccess;

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

    uchar m_currentCmdListSelector = 0;
    QByteArray m_rawCyclicCmdMem; // unsere dsp programm listen
    QByteArray m_rawInterruptCmdMem;
    QSocketNotifier* m_pNotifier = nullptr;

    ZDspClientContainer m_zdspClientContainer;

    ulong UserWorkSpaceGlobalSegmentAdr;
    LogStatisticsAsyncInt m_dspInterruptLogStatistics;
    bool m_outputHealthLogs;

    bool resetDsp();
    bool bootDsp();
    bool setSamplingSystem();

    QString sendCommand2Dsp(const QString &dspCmdParLine);
    QString getLcaAndDspVersion();
    QString getSamplingSystemSetup();
    QString getDspCommandStat();
    QString setDspCommandStat(const QString &scpiParam);
    QString runDspTest(const QString &scpiParam);
    QString startTriggerIntListHKSK(const QString &scpiParam, int dspInterruptId);

    // die routinen für das status modell
    QString getDeviceStatus();
    QString getDspStatus();

    // die routinen für das measure modell
    QString loadCmdListAllClients();

    bool compileCmdListsForAllClientsToRawStream(QString& errs);
    bool uploadCommandLists();
    void flipCommandListSelector();
    bool writeDspCmdListsToDevNode();
    bool setDspType();
    int readMagicId();
    bool Test4HWPresent();
    bool Test4DspRunning();
    void executeCommandProto(VeinTcp::TcpPeer* peer, std::shared_ptr<google::protobuf::Message> cmd);
    QString m_sDspBootPath;

    QStateMachine* m_pInitializationMachine;
    QState* m_stateconnect2RM;
    QState* m_stateconnect2RMError;
    QState* m_stateSendRMIdentAndRegister;
    RMConnection* m_pRMConnection = nullptr;
    ResourceRegisterTransaction m_resourceRegister;
    ConsoleServer m_telnetServer;

    int m_retryRMConnect;
    QTimer m_retryTimer;
    TimerTemplateQtPtr m_periodicLogTimer;
};

#endif
