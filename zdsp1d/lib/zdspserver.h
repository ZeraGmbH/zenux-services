#ifndef ZDSP1D_H
#define ZDSP1D_H

#include "abstractfactoryzdspsupport.h"
#include "consoleserver.h"
#include "dspvardevicenodeinout.h"
#include "dspsettings.h"
#include "zdspclientcontainer.h"
#include "scpiconnection.h"
#include "settingscontainer.h"
#include <timertemplateqt.h>
#include <logstatisticsasyncint.h>
#include <vtcp_server.h>
#include <xiqnetwrapper.h>
#include <abstracttcpnetworkfactory.h>
#include <QStringList>
#include <QSocketNotifier>
#include <QByteArray>
#include <QStateMachine>

class ZDspServer: public ScpiConnection
{
    Q_OBJECT
public:
    ZDspServer(SettingsContainerPtr settings,
               AbstractFactoryZdspSupportPtr zdspSupportFactory,
               VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
               bool outputHealthLogs);
    virtual ~ZDspServer();
    QString getServerVersion();
    QString getDspDeviceNode();

    const QList<ZdspClient*> &getClients() const;
    bool compileCmdListsForAllClientsToBinaryStream(QString& errs,
                                                    QByteArray &rawCyclicCmdMemOut,
                                                    QByteArray &rawInterruptCmdMemOut) const;
    bool compileCmdListsForAllClientsToCompileLists(QString& errs,
                                                    QList<DspCmdWithParamsCompiled> &compiledCyclicOut,
                                                    QList<DspCmdWithParamsCompiled> &compiledInterruptOut) const;

    int getVarMemAvailable() const; // global & local - they share more or less an area
    int getVarMemAlignedAvailable() const;
    int getVarMemOccupied(DspSegmentType segment) const;

    int getProgMemCyclicAvailable() const;
    int getProgMemCyclicOccupied() const;
    int getProgMemInterruptAvailable() const;
    int getProgMemInterruptOccupied() const;

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

    void outputLogs();
private:
    void init();
    void initSCPIConnection();
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
    void doFinalSetupSteps();

    friend class TestZdsp1dForVarAccess;

    QString handleScpiInterfaceRead(const QString &scpiInput);
    QString handleSetDspSuperClient(const ZdspClient* client);
    void outputDspRunState();
    void outputAndResetTransactionsLogs();
    void openTelnetScpi();

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

    DspCmdWithParamsCompiled genClientStartAddressCmd(ulong userMemOffset, ZdspClient* client, AbstractDspCompilerSupportPtr compilerSupport, bool &ok) const;
    bool uploadCommandLists();
    void flipCommandListSelector();
    bool writeDspCmdListsToDevNode();
    bool setDspType();
    int readMagicId();
    bool Test4HWPresent();
    bool Test4DspRunning();
    void executeCommandProto(VeinTcp::TcpPeer* peer, std::shared_ptr<google::protobuf::Message> cmd);

    Zera::XMLConfig::cReader m_xmlConfigReader;
    cDSPSettings m_dspSettings;
    SettingsContainerPtr m_settings;
    AbstractFactoryZdspSupportPtr m_zdspSupportFactory;
    DspVarDeviceNodeInOut m_dspInOut;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    VeinTcp::TcpServer m_protoBufServer;
    XiQNetWrapper m_protobufWrapper;

    uchar m_currentCmdListSelector = 0;
    QByteArray m_rawCyclicCmdMem; // unsere dsp programm listen
    QByteArray m_rawInterruptCmdMem;
    QSocketNotifier* m_pNotifier = nullptr;

    ZDspClientContainer m_zdspClientContainer;

    ulong m_userWorkSpaceAlignedSegmentStartAdr;
    LogStatisticsAsyncInt m_dspInterruptLogStatistics;
    bool m_outputHealthLogs;

    QStateMachine* m_pInitializationMachine;
    ConsoleServer m_telnetServer;

    TimerTemplateQtPtr m_periodicLogTimer;
};

#endif
