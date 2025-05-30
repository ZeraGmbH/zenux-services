#include "zdspserver.h"
#include "zdspclient.h"
#include "dspapi.h"
#include "dspcmdcompiler.h"
#include "commonscpimethods.h"
#include "devicenodedsp.h"
#include "zscpi_response_definitions.h"
#include <timerfactoryqt.h>
#include <scpinodestaticfunctions.h>
#include <QDebug>
#include <QCoreApplication>
#include <QFinalState>
#include <QDataStream>
#include <QTcpServer>
#include <QDomDocument>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#define ServerName "zdsp1d"
#define ServerVersion "V1.11"

extern TMemSection dm32DspWorkspace;
extern TMemSection dm32DialogWorkSpace;
extern TMemSection dm32UserWorkSpace;
extern TMemSection dm32CmdList;
extern TMemSection symbConsts1;

extern TDspVar CmdListVar;
extern TDspVar UserWorkSpaceVar;

constexpr int loggingIntervalMs = 10000;

static char devavail[6] = "avail";
static char devnavail[10]= "not avail";

static char dsprunning[8] = "running";
static char dspnrunning[12]= "not running";


int pipeFileDescriptorZdsp1[2];
static void SigHandler(int)
{
    const static char pipeFDBuf[2] = "I";
    write(pipeFileDescriptorZdsp1[1], pipeFDBuf, 1);
}


struct sigaction sigActionZdsp1;
// sigset_t mySigmask, origSigmask;


const ServerParams ZDspServer::defaultParams {ServerName, ServerVersion, "/etc/zera/zdsp1d/zdsp1d.xsd", "/etc/zera/zdsp1d/zdsp1d.xml"};

ZDspServer::ZDspServer(SettingsContainerPtr settings,
                       AbstractFactoryDeviceNodeDspPtr deviceNodeFactory,
                       VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory) :
    ScpiConnection(std::make_shared<cSCPI>()),
    m_dspSettings(&m_xmlConfigReader),
    m_settings(std::move(settings)),
    m_deviceNodeFactory(deviceNodeFactory),
    m_dspInOut(deviceNodeFactory),
    m_tcpNetworkFactory(tcpNetworkFactory),
    m_protoBufServer(tcpNetworkFactory),
    m_dspInterruptLogStatistics(10000),
    m_pRMConnection(new RMConnection(m_settings->getEthSettings()->getRMIPadr(),
                                     m_settings->getEthSettings()->getPort(EthSettings::resourcemanager),
                                     m_tcpNetworkFactory)),
    m_resourceRegister(m_pRMConnection)
{
    doConfiguration();
    init();
}

void ZDspServer::init()
{
    m_pInitializationMachine = new QStateMachine(this);

    QState* stateCONF = new QState(); // we start from here
    QFinalState* stateFINISH = new QFinalState(); // and here we finish on any error condition

    stateCONF->addTransition(this, &ZDspServer::abortInit, stateFINISH); // from anywhere we arrive here if some error

    QState* statesetupServer = new QState(stateCONF); // we setup our server now
    m_stateconnect2RM = new QState(stateCONF); // we connect to resource manager
    m_stateconnect2RMError = new QState(stateCONF);
    m_stateSendRMIdentAndRegister = new QState(stateCONF); // we send ident. to rm and register our resources

    stateCONF->setInitialState(statesetupServer);

    statesetupServer->addTransition(this, &ZDspServer::sigServerIsSetUp, m_stateconnect2RM);

    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);

    QObject::connect(statesetupServer, &QAbstractState::entered, this, &ZDspServer::doSetupServer);
    QObject::connect(m_stateconnect2RM, &QAbstractState::entered, this, &ZDspServer::doConnect2RM);
    QObject::connect(m_stateconnect2RMError, &QAbstractState::entered, this, &ZDspServer::connect2RMError);
    QObject::connect(m_stateSendRMIdentAndRegister, &QAbstractState::entered, this, &ZDspServer::doIdentAndRegister);
    QObject::connect(stateFINISH, &QAbstractState::entered, this, &ZDspServer::doCloseServer);

    m_pInitializationMachine->start();

    connect(&m_dspInterruptLogStatistics, &LogStatisticsAsyncInt::sigNewStatistics, [](int min, int max, float avg, int interruptCnt) {
        qInfo("DSP Interrupts per Linux interrupt min: %i, max: %i, mean: %.1f; Total Linux Interrupts: %i",
              min, max, avg, interruptCnt);
    });
}

ZDspServer::~ZDspServer()
{
    delete m_pRMConnection;
    delete m_telnetServer;
    resetDsp(); // we reset the dsp when we close the server
    AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
    deviceNode->close();
    close(pipeFileDescriptorZdsp1[0]);
    close(pipeFileDescriptorZdsp1[1]);
}

void ZDspServer::doConfiguration()
{
    if(pipe(pipeFileDescriptorZdsp1) == -1)
        qCritical("Abort, could not open pipe");
    else {
        fcntl( pipeFileDescriptorZdsp1[1], F_SETFL, O_NONBLOCK);
        fcntl( pipeFileDescriptorZdsp1[0], F_SETFL, O_NONBLOCK);
        m_pNotifier = new QSocketNotifier(pipeFileDescriptorZdsp1[0], QSocketNotifier::Read, this);
        connect(m_pNotifier, &QSocketNotifier::activated, this, &ZDspServer::DspIntHandler);
        ServerParams params = m_settings->getServerParams();
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
                &m_dspSettings, &cDSPSettings::configXMLInfo);
        if(!m_xmlConfigReader.loadXMLFile(params.xmlFile))
            qCritical("Abort: Could not open xml file '%s", qPrintable(params.xmlFile));
    }
}

void ZDspServer::doSetupServer()
{
    qInfo("Starting doSetupServer");
    initSCPIConnection(QString());
    m_sDspBootPath = m_dspSettings.getBootFile();

    connect(&m_protoBufServer, &VeinTcp::TcpServer::sigClientConnected,
            this, &ZDspServer::onProtobufClientConnected);

    QString dspDevNodeName = getDspDeviceNode(); // we try to open the dsp device
    AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
    if (deviceNode->open(dspDevNodeName) < 0) {
        qCritical("Abort: Could not open dsp device node '%s", qPrintable(dspDevNodeName));
        emit abortInit();
    }
    else {
        sigActionZdsp1.sa_handler = &SigHandler; // signal handler einrichten
        sigemptyset(&sigActionZdsp1.sa_mask);
        sigActionZdsp1. sa_flags = SA_RESTART;
        sigActionZdsp1.sa_restorer = NULL;
        sigaction(SIGIO, &sigActionZdsp1, NULL); // handler für sigio definieren
        deviceNode->enableFasync();
        m_retryRMConnect = 100;
        m_retryTimer.setSingleShot(true);
        connect(&m_retryTimer, &QTimer::timeout, this, &ZDspServer::sigServerIsSetUp);

        if (setDspType()) { // interrogate the mounted dsp device type and bootfile match
            if (m_dspSettings.isBoot()) { // normally dsp gets booted by dsp server
                if (resetDsp() && bootDsp()) { // and try to reset and then boot it
                    if (setSamplingSystem()) { // now we try to set the dsp's sampling system
                        // our resource manager connection must be opened after configuration is done
                        m_stateconnect2RM->addTransition(m_pRMConnection, &RMConnection::connected, m_stateSendRMIdentAndRegister);
                        m_stateconnect2RM->addTransition(m_pRMConnection, &RMConnection::connectionRMError, m_stateconnect2RMError);
                        m_stateconnect2RMError->addTransition(this, &ZDspServer::sigServerIsSetUp, m_stateconnect2RM);
                        emit sigServerIsSetUp(); // so we enter state machine's next state
                    }
                    else {
                        qCritical("Abort, dsp device set sampling system error");
                        emit abortInit();
                    }
                }
                else {
                    qCritical("Abort, dsp device boot error");
                    emit abortInit();
                }
            }
            else { // but for debugging purpose dsp is booted by ice
                m_stateconnect2RM->addTransition(m_pRMConnection, &RMConnection::connected, m_stateSendRMIdentAndRegister);
                m_stateconnect2RM->addTransition(m_pRMConnection, &RMConnection::connectionRMError, m_stateconnect2RMError);
                m_stateconnect2RMError->addTransition(this, &ZDspServer::sigServerIsSetUp, m_stateconnect2RM);
                emit sigServerIsSetUp(); // so we enter state machine's next state
            }
        }
        else {
            qCritical("Abort, dsp device and bootfile mismatch");
            emit abortInit();
        }
    }
}

void ZDspServer::doCloseServer()
{
    QCoreApplication::instance()->exit(-1);
}

void ZDspServer::doConnect2RM()
{
    qInfo("Starting doConnect2RM");
    m_pRMConnection->connect2RM();
}

void ZDspServer::connect2RMError()
{
    m_retryRMConnect--;
    if (m_retryRMConnect == 0) {
        qCritical("Connect to resourcemanager failed: Abort");
        emit abortInit();
    }
    else {
        qWarning("Connect to resourcemanager failed: Retry");
        m_retryTimer.start(200);
    }
}

void ZDspServer::doIdentAndRegister()
{
    qInfo("Starting doIdentAndRegister");
    m_pRMConnection->SendIdent(ServerName);

    EthSettings *ethSettings = m_settings->getEthSettings();
    quint32 port = ethSettings->getPort(EthSettings::protobufserver);

    connect(&m_resourceRegister, &ResourceRegisterTransaction::registerRdy, this, &ZDspServer::onResourceReady);
    m_resourceRegister.register1Resource(QString("DSP;DSP1;;ADSP Signal Processor;%1;").arg(port));
    TDspVar* pDspVar = &CmdListVar;
    m_resourceRegister.register1Resource(QString("DSP1;PGRMEMI;%1;DSP ProgramMemory(Interrupt);%2;").arg(pDspVar->size).arg(port));
    pDspVar++;
    m_resourceRegister.register1Resource(QString("DSP1;PGRMEMC;%1;DSP ProgramMemory(Cyclic);%2;").arg(pDspVar->size).arg(port));
    pDspVar = &UserWorkSpaceVar;
    m_resourceRegister.register1Resource(QString("DSP1;USERMEM;%1;DSP UserMemory;%2;").arg(pDspVar->size).arg(port));
}

void ZDspServer::onResourceReady()
{
    disconnect(&m_resourceRegister, &ResourceRegisterTransaction::registerRdy, this, &ZDspServer::onResourceReady);
    EthSettings *ethSettings = m_settings->getEthSettings();
    m_protoBufServer.startServer(ethSettings->getPort(EthSettings::protobufserver));
    openTelnetScpi();
    m_periodicLogTimer = TimerFactoryQt::createPeriodic(loggingIntervalMs);
    connect(m_periodicLogTimer.get(), &TimerTemplateQt::sigExpired,
            this, &ZDspServer::outputLogs);
    m_periodicLogTimer->start();
}

void ZDspServer::outputDspRunState()
{
    QString dspStatus = getDspStatus();
    QString message = QString("DSP is %1").arg(dspStatus);
    if(dspStatus != dsprunning)
        qWarning("%s", qPrintable(message));
}

void ZDspServer::outputAndResetTransactionsLogs()
{
    int readTransactions = AbstractDspDeviceNode::getReadTransactions();
    int writeTransactions = AbstractDspDeviceNode::getWriteTransactions();
    AbstractDspDeviceNode::resetAllTransactions();
    QString message = QString("DSP transactions: Read: %1 / Write %2").arg(readTransactions).arg(writeTransactions);
    qInfo("%s", qPrintable(message));
}

void ZDspServer::openTelnetScpi()
{
    EthSettings *ethSettings = m_settings->getEthSettings();
    if (ethSettings->isSCPIactive()) {
        m_telnetServer = new QTcpServer(this);
        m_telnetServer->setMaxPendingConnections(1); // we only accept 1 client to connect
        connect(m_telnetServer, &QTcpServer::newConnection, this, &ZDspServer::onTelnetClientConnected);
        m_telnetServer->listen(QHostAddress::AnyIPv4, ethSettings->getPort(EthSettings::scpiserver));
    }
}

void ZDspServer::outputLogs()
{
    outputDspRunState();
    outputAndResetTransactionsLogs();
}

enum SCPICmdType  {
    // die routinen für das system modell
    scpiDspCommandStatGetSet,
    scpiTriggerIntListHKSK,
    scpiTriggerIntListALL,
    scpiGetDeviceVersion,
    scpiGetServerVersion,
    scpiSamplingSystemGetSet,
    scpiRunTest,

    // die routinen für das status modell
    scpiGetDeviceLoadAct,
    scpiGetDeviceLoadMax,
    scpiResetDeviceLoadMax,
    scpiGetDspStatus,
    scpiGetDeviceStatus,

    // die routinen für das measure modell
    scpiUnloadCmdList,
    scpiUnloadCmdListAllClients,
    scpiLoadCmdList,
    scpiRavListSet,
    scpiCmdIntListSet,
    scpiCmdCycListSet,
    scpiReadActualValues, // AKA data acquisition

    // die routinen für das memory modell
    scpiDspMemoryRead,
    scpiDspMemoryWrite,

    // common
    scpiInterfaceRead
};

void ZDspServer::initSCPIConnection(QString leadingNodes)
{
    Q_UNUSED(leadingNodes)

    addDelegate("SYSTEM:INTERFACE", "READ", SCPI::isQuery, m_scpiInterface, scpiInterfaceRead);
    addDelegate("SYSTEM:VERSION", "DEVICE", SCPI::isQuery, m_scpiInterface, scpiGetDeviceVersion);
    addDelegate("SYSTEM:VERSION", "SERVER", SCPI::isQuery, m_scpiInterface, scpiGetServerVersion);
    addDelegate("SYSTEM:DSP", "SAMPLING", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, scpiSamplingSystemGetSet);
    addDelegate("SYSTEM:DSP", "TEST", SCPI::isCmdwP, m_scpiInterface, scpiRunTest);
    addDelegate("SYSTEM:DSP:COMMAND", "STAT", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, scpiDspCommandStatGetSet);
    addDelegate("SYSTEM:DSP:TRIGGER:INTLIST", "ALL", SCPI::isCmd, m_scpiInterface, scpiTriggerIntListALL);
    addDelegate("SYSTEM:DSP:TRIGGER:INTLIST", "HKSK", SCPI::isCmdwP, m_scpiInterface, scpiTriggerIntListHKSK);

    addDelegate("MEMORY", "READ", SCPI::isCmdwP, m_scpiInterface, scpiDspMemoryRead);
    addDelegate("MEMORY", "WRITE", SCPI::isCmdwP, m_scpiInterface, scpiDspMemoryWrite);

    addDelegate("", "MEASURE", SCPI::isCmdwP, m_scpiInterface, scpiReadActualValues);
    addDelegate("MEASURE:LIST", "RAVLIST", SCPI::isCmdwP, m_scpiInterface, scpiRavListSet);
    addDelegate("MEASURE:LIST", "INTLIST", SCPI::isCmdwP, m_scpiInterface, scpiCmdIntListSet);
    addDelegate("MEASURE:LIST", "CYCLIST", SCPI::isCmdwP, m_scpiInterface, scpiCmdCycListSet);
    addDelegate("MEASURE:LIST", "SET", SCPI::isCmdwP, m_scpiInterface, scpiLoadCmdList);
    addDelegate("MEASURE:LIST", "CLEAR", SCPI::isCmdwP, m_scpiInterface, scpiUnloadCmdList);
    addDelegate("MEASURE:LIST", "CLALL", SCPI::isCmd, m_scpiInterface, scpiUnloadCmdListAllClients);

    addDelegate("STATUS", "DEVICE", SCPI::isQuery, m_scpiInterface, scpiGetDeviceStatus);
    addDelegate("STATUS", "DSP", SCPI::isQuery, m_scpiInterface, scpiGetDspStatus);
    addDelegate("STATUS:DSP:LOAD", "ACTUAL", SCPI::isQuery, m_scpiInterface, scpiGetDeviceLoadAct);
    addDelegate("STATUS:DSP:LOAD", "MAXIMUM", SCPI::isQuery, m_scpiInterface, scpiGetDeviceLoadMax);
    addDelegate("STATUS:DSP:LOAD:MAXIMUM", "RESET", SCPI::isCmdwP, m_scpiInterface, scpiResetDeviceLoadMax);

    connect(this, &ScpiConnection::cmdExecutionDone, this, &ZDspServer::sendProtoAnswer);
}

void ZDspServer::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    ZdspClient* client = m_zdspClientContainer.findClient(protoCmd->m_clientId);
    switch (cmdCode)
    {
    case scpiInterfaceRead:
        protoCmd->m_sOutput = handleScpiInterfaceRead(protoCmd->m_sInput);
        break;
    case scpiGetDeviceVersion:
        protoCmd->m_sOutput = getLcaAndDspVersion();
        break;
    case scpiGetServerVersion:
        protoCmd->m_sOutput = getServerVersion();
        break;
    case scpiSamplingSystemGetSet:
        if(cmd.isQuery())
            protoCmd->m_sOutput = getSamplingSystemSetup();
        else
            protoCmd->m_sOutput = sendCommand2Dsp(QString("DSPCMDPAR,2,%1;").arg(cmd.getParam()));
        break;
    case scpiRunTest:
        protoCmd->m_sOutput = runDspTest(cmd.getParam());
        break;
    case scpiDspCommandStatGetSet:
        if(cmd.isQuery())
            protoCmd->m_sOutput = getDspCommandStat();
        else
            protoCmd->m_sOutput = setDspCommandStat(cmd.getParam());
        break;
    case scpiDspMemoryRead:
        protoCmd->m_sOutput = m_dspInOut.readDspVarList(cmd.getParam(), &client->m_dspVarResolver);
        break;
    case scpiDspMemoryWrite:
        if(m_dspInOut.writeDspVars(cmd.getParam(), &client->m_dspVarResolver))
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
        else
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::errexec];
        break;
    case scpiRavListSet:
        if(client->setRawActualValueList(cmd.getParam()))
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
        else
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::errexec];
        break;
    case scpiCmdCycListSet:
        client->setCmdListDef(cmd.getParam());
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack]; // ist erstmal ok, wird später beim SET kommando geprüft
        break;
    case scpiCmdIntListSet:
        client->setCmdForIrqListDef(cmd.getParam());
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack]; // ist erstmal ok, wird später beim SET kommando geprüft
        break;
    case scpiLoadCmdList:
        protoCmd->m_sOutput = loadCmdListAllClients();
        if (protoCmd->m_sOutput != ZSCPI::scpiAnswer[ZSCPI::ack]) {
            m_zdspClientContainer.delClient(client->getProtobufClientId());
            client = nullptr;
        }
        break;
    case scpiUnloadCmdList:
        m_zdspClientContainer.delClient(client->getProtobufClientId());
        client = nullptr;
        protoCmd->m_sOutput = loadCmdListAllClients();
        break;
    case scpiUnloadCmdListAllClients:
        m_zdspClientContainer.delAllClients();
        protoCmd->m_sOutput = loadCmdListAllClients();
        break;
    case scpiReadActualValues:
        protoCmd->m_sOutput = client->readActValues(cmd.getParam());
        break;
    case scpiGetDeviceStatus:
        protoCmd->m_sOutput = getDeviceStatus();
        break;
    case scpiGetDspStatus:
        protoCmd->m_sOutput = getDspStatus();
        break;
    case scpiGetDeviceLoadAct:
        // ab "BUSY"  1 wort lesen
        protoCmd->m_sOutput = m_dspInOut.readDspVarList("BUSY,1;", &client->m_dspVarResolver);
        break;
    case scpiGetDeviceLoadMax:
        // ab "BUSYMAX"  1 wort lesen
        protoCmd->m_sOutput = m_dspInOut.readDspVarList("BUSYMAX,1;", &client->m_dspVarResolver);
        break;
    case scpiResetDeviceLoadMax:
        if(m_dspInOut.writeDspVars("BUSYMAX,0.0", &client->m_dspVarResolver))
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
        else
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::errexec];
        break;
    case scpiTriggerIntListALL:
        protoCmd->m_sOutput = sendCommand2Dsp("DSPCMDPAR,1;");
        break;
    case scpiTriggerIntListHKSK: {
        int socketNum = client->getDspInterruptId();
        protoCmd->m_sOutput = startTriggerIntListHKSK(cmd.getParam(0), socketNum);
        break;
    }
    }
}

QString ZDspServer::handleScpiInterfaceRead(const QString &scpiInput)
{
    QString xmlScpi = CommonScpiMethods::handleScpiInterfaceRead(m_scpiInterface, scpiInput);
    QDomDocument domDoc("SCPIModel");
    domDoc.setContent(xmlScpi);

    QDomElement rootElem = domDoc.documentElement();
    QDomElement modelsElem = rootElem.firstChildElement("MODELS");

    return domDoc.toString();
}

bool ZDspServer::resetDsp()
{
    AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
    return deviceNode->dspReset();
}

bool ZDspServer::bootDsp()
{
    AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
    return deviceNode->dspBoot(m_sDspBootPath);
}

bool ZDspServer::setSamplingSystem()
{
    for (int i = 0; i < 10; i++) { // we try max. 10 times to set .... this should work
        QString ret = sendCommand2Dsp(QString("DSPCMDPAR,2,%1,%2,%3;")
                                       .arg(m_dspSettings.getChannelNr())
                                       .arg(m_dspSettings.getSamplesSignalPeriod())
                                       .arg(m_dspSettings.getsamplesMeasurePeriod()));
        if (ret == ZSCPI::scpiAnswer[ZSCPI::ack])
            return true;
        usleep(10000); // give dsp a bit time before next try
    }
    return false;
}

QString ZDspServer::sendCommand2Dsp(const QString &dspCmdParLine)
{
    DspVarResolver dspSystemVarResolver;
    int ack;
    if(!m_dspInOut.readOneDspVarInt("DSPACK", ack, &dspSystemVarResolver))
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    if(ack == InProgress)
        return ZSCPI::scpiAnswer[ZSCPI::busy];
    if(!m_dspInOut.writeDspVars("DSPACK,0;", &dspSystemVarResolver) )
        return ZSCPI::scpiAnswer[ZSCPI::errexec]; // reset acknowledge
    if(!m_dspInOut.writeDspVars(dspCmdParLine, &dspSystemVarResolver))
        return ZSCPI::scpiAnswer[ZSCPI::errexec];

    AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
    deviceNode->dspRequestInt(); // interrupt beim dsp auslösen
    return ZSCPI::scpiAnswer[ZSCPI::ack]; // sofort fertig melden ....sync. muss die applikation
}

QString ZDspServer::getSamplingSystemSetup()
{
    DspVarResolver dspSystemVarResolver;
    int measmeasmeasChannelCount = 0;
    if (!m_dspInOut.readOneDspVarInt("NCHANNELS", measmeasmeasChannelCount, &dspSystemVarResolver))
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    int samplesPerMeasPeriod = 0;
    if (!m_dspInOut.readOneDspVarInt("NSPERIOD", samplesPerMeasPeriod, &dspSystemVarResolver))
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    int samplesPerSignalPeriod = 0;
    if (!m_dspInOut.readOneDspVarInt("NSMEAS", samplesPerSignalPeriod, &dspSystemVarResolver))
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    return QString("%1,%2,%3").arg(measmeasmeasChannelCount).arg(samplesPerMeasPeriod).arg(samplesPerSignalPeriod);
}

QString ZDspServer::getDspCommandStat()
{
    int stat;
    DspVarResolver dspSystemVarResolver;
    if(!m_dspInOut.readOneDspVarInt("DSPACK", stat, &dspSystemVarResolver))
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    else
        return QString("%1").arg(stat);
}

QString ZDspServer::setDspCommandStat(const QString &scpiParam)
{
    DspVarResolver dspSystemVarResolver;
    if(!m_dspInOut.writeDspVars(QString("DSPACK,%1;").arg(scpiParam), &dspSystemVarResolver) )
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    else
        return ZSCPI::scpiAnswer[ZSCPI::ack];
}

QString ZDspServer::runDspTest(const QString &scpiParam)
{
    // Unexpected command (parameter) format: 'SYSTEM:DSP:TEST <test-type [0;1]> <count>;'
    QString scpiParamNoSemicolon = scpiParam;
    scpiParamNoSemicolon.remove(";");
    const QStringList params = scpiParamNoSemicolon.split(" ", Qt::SkipEmptyParts);
    if(params.count() != 2)
        return ZSCPI::scpiAnswer[ZSCPI::errval];
    bool ok;
    int tmode = params[0].toInt(&ok);
    int nr = 0;
    bool tstart = false;
    if ((ok) && ( (tmode>=0) && (tmode<2) )) {
        nr = params[1].toInt(&ok);
        if ((ok) && ( (nr>=0) && (nr<1000) ))
            tstart = true;
    }
    QString ret;
    if (tstart == true) {
        int errcount = 0;
        switch (tmode)
        {
        case 0:
            for (int i=0; i<nr; i++) {
                resetDsp();
                int j;
                for (j=0; j< 100; j++) {
                    usleep(1000);
                    if (Test4DspRunning() == false)
                        break;
                }
                if (j==100)
                    errcount++;
                else {
                    bootDsp();
                    usleep(1000);
                    if (Test4DspRunning() == false)
                        errcount++;
                }
                ret = QString("Test booting dsp %1 times, errors %2").arg(nr).arg(errcount);
            }
            break;

        case 1:
            const int n = 10000;
            int i,j;
            ulong faultadr;
            int bw, br, br2;
            QByteArray ba; // wir werden 10000 floats in das array schreiben
            QByteArray ba2; // zurückgelesene daten
            ba.resize(n*4);
            ba2.resize(n*4);
            char byte = 0;
            for (i=0; i<n*4;i++) {
                ba[i] = byte;
                byte = (byte +1) % 256;
            }
            DspVarResolver dspSystemVarResolver;
            QString sadr  = "UWSPACE";
            ulong adr = dspSystemVarResolver.getVarAddress(sadr) ;
            AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
            for (i=0; i< nr; i++) {
                if(!deviceNode->write(adr, ba.data(), n*4 )) {
                    ret = QString("Test write/read dsp data, dev write fault");
                    break; // fehler beim schreiben
                }
                if (deviceNode->lseek(adr) < 0) {
                    ret = QString("Test write/read dsp data, dev seek fault");
                    break; // file positionieren
                }
                if (deviceNode->read(ba2.data(), n*4) < 0) {
                    ret = QString("Test write/read dsp data, dev read fault");
                    break; // fehler beim schreiben
                }
                bool err = false;
                for (j=0; j<n*4; j++) {
                    if (ba[j] != ba2[j]) {
                        bw = ba[j]; // das geschriebene byte
                        br = ba2[j]; // das gelesene byte
                        faultadr = adr + j;
                        deviceNode->read(ba2.data(), n*4);
                        br2 = ba2[j];
                        err = true;
                    }
                    if (err)
                        break;
                }
                if (err) {
                    ret = QString("Test write/read dsp data, data fault adress %1, write %2, read1 %3, read2 %4").arg(faultadr,16).arg(bw,16).arg(br,16).arg(br2,16);
                    break; // file positionieren
                }
            }
            if (i==nr)
                ret = QString("Test write/read dsp data, %1 times %2 bytes transferred, no errors").arg(nr).arg(n*4);
            break;
        }
    }
    else
        ret = ZSCPI::scpiAnswer[ZSCPI::errval]; // fehler wert
    return ret;
}

QString ZDspServer::startTriggerIntListHKSK(const QString &scpiParam, int dspInterruptId)
{
    ulong par = scpiParam.toULong();
    par = (par & 0xFFFF )| (dspInterruptId << 16);
    return sendCommand2Dsp(QString("DSPCMDPAR,4,%1;").arg(par)); // liste mit prozessNr u. HKSK
}

QString ZDspServer::getLcaAndDspVersion()
{
    AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
    // LCA
    int rawLcaVersion = deviceNode->lcaRawVersion();
    if ( rawLcaVersion < 0 )
        return ZSCPI::scpiAnswer[ZSCPI::errexec]; // fehler bei der ausführung
    // DSP
    DspVarResolver dspSystemVarResolver;
    QString dspVer = m_dspInOut.readDspVarList("VNR,1;", &dspSystemVarResolver);  // ab "VNR"  1 wort lesen
    dspVer = dspVer.section(':',1,1);
    dspVer.remove(';');
    double dspVersionFloat = dspVer.toDouble();
    return QString("DSPLCA: V%1.%2;DSP V%3").arg((rawLcaVersion >>8) & 0xff).arg(rawLcaVersion & 0xff).arg(dspVersionFloat, 0, 'f', 2);
}

QString ZDspServer::getServerVersion()
{
    return QString("%1 %2").arg(ServerName, ServerVersion);
}

QString ZDspServer::getDspDeviceNode()
{
    return m_settings->getFpgaSettings()->getDspDeviceNode();
}

ZdspClient *ZDspServer::addClientForTest()
{
    m_zdspClientContainer.addClient(nullptr, QByteArray(), m_deviceNodeFactory);
    return m_zdspClientContainer.findClient(QByteArray());
}

QString ZDspServer::getDspStatus()
{
    if ( Test4DspRunning() )
        return dsprunning;
    else
        return dspnrunning;
}

QString ZDspServer::getDeviceStatus()
{
    if ( Test4HWPresent() )
        return devavail;
    else
        return devnavail;
}

QDataStream& operator<<(QDataStream& ds,DspCmdWithParamsRaw c)
{
    ds << (quint32) c.w[0] << (quint32) c.w[1];
    return ds;
}

void ZDspServer::DspIntHandler(int)
{
    char dummy[2];
    read(pipeFileDescriptorZdsp1[0], dummy, 1); // first we read the pipe

    const QList<ZdspClient*> clientList = m_zdspClientContainer.getClientList();
    if (!clientList.isEmpty()) { // wenn vorhanden nutzen wir immer den 1. client zum lesen
        ZdspClient *client = clientList.first();
        QByteArray ba;
        if (m_dspInOut.readOneDspVar("CTRLCMDPAR,20", &ba, &client->m_dspVarResolver)) { // 20 worte lesen
            const ulong* pardsp = reinterpret_cast<ulong*>(ba.data());
            int n = pardsp[0]; // anzahl der interrupts
            m_dspInterruptLogStatistics.addValue(n);
            if (n > 20)   // in the moment magic nr
                qInfo ("Number of interrupts in a package: %i exceeds upper limit!", n);
            else {
                for (int i = 1; i < (n+1); i++) {
                    int process = pardsp[i] >> 16;
                    ZdspClient *clientToNotify = m_zdspClientContainer.findClient(process);
                    if (clientToNotify) {
                        const QString dspIntStr = QString("DSPINT:%1").arg(pardsp[i] & 0xFFFF);

                        ProtobufMessage::NetMessage protobufIntMessage;
                        ProtobufMessage::NetMessage::NetReply *intMessage = protobufIntMessage.mutable_reply();

                        intMessage->set_body(dspIntStr.toStdString());
                        intMessage->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);

                        const QByteArray proxyConnectionId = clientToNotify->getProtobufClientId();
                        protobufIntMessage.set_clientid(proxyConnectionId.data(), proxyConnectionId.size());
                        protobufIntMessage.set_messagenr(0); // interrupt

                        clientToNotify->getVeinPeer()->sendMessage(m_protobufWrapper.protobufToByteArray(protobufIntMessage));
                    }
                }
            }
        }
        m_dspInOut.writeDspVars(QString("CTRLACK,%1;").arg(CmdDone), &client->m_dspVarResolver); // jetzt in jedem fall acknowledge
    }
    else {
        DspVarResolver dspSystemVarResolver;
        m_dspInOut.writeDspVars(QString("CTRLACK,%1;").arg(CmdDone), &dspSystemVarResolver); // und rücksetzen
    }
}


bool ZDspServer::compileCmdListsForAllClientsToRawStream(QString &errs)
{
    bool ok;

    m_rawCyclicCmdMem.clear();
    m_rawInterruptCmdMem.clear();
    QDataStream cycCmdMemStream(&m_rawCyclicCmdMem, QIODevice::Unbuffered | QIODevice::ReadWrite);
    cycCmdMemStream.setByteOrder(QDataStream::LittleEndian);
    QDataStream intCmdMemStream(&m_rawInterruptCmdMem, QIODevice::Unbuffered | QIODevice::ReadWrite);
    intCmdMemStream.setByteOrder(QDataStream::LittleEndian);

    DspCmdWithParamsRaw cmd;
    const QList<ZdspClient*> clientList = m_zdspClientContainer.getClientList();
    if (clientList.count() > 0) {
        ZdspClient* firstClient = clientList.at(0);
        DspCmdCompiler firstCompiler(&firstClient->m_dspVarResolver, firstClient->getDspInterruptId());
        cmd = firstCompiler.compileOneCmdLineZeroAligned(QString("DSPMEMOFFSET(%1)").arg(dm32DspWorkspace.m_startAddress),
                                                         &ok);
        cycCmdMemStream << cmd;
        ulong userMemOffset = dm32UserWorkSpace.m_startAddress;
        for (int i = 0; i < clientList.count(); i++) {
            ZdspClient* client = clientList.at(i);
            DspCmdCompiler compiler(&client->m_dspVarResolver, client->getDspInterruptId());
            cmd = compiler.compileOneCmdLineZeroAligned(QString("USERMEMOFFSET(%1)").arg(userMemOffset),
                                                        &ok);
            cycCmdMemStream << cmd;
            intCmdMemStream << cmd;

            if (!client->GenCmdLists(errs, userMemOffset, UserWorkSpaceGlobalSegmentAdr))
                return false;

            // relokalisieren der daten im dsp
            userMemOffset += client->relocalizeUserMemSectionVars(userMemOffset, UserWorkSpaceGlobalSegmentAdr);

            QList<DspCmdWithParamsRaw> cycCmdList = client->GetDspCmdList();
            for (int j = 0; j < cycCmdList.size(); j++)
                cycCmdMemStream << cycCmdList[j];
            QList<DspCmdWithParamsRaw> intCmdList = client->GetDspIntCmdList();
            for (int j = 0; j < intCmdList.size(); j++)
                intCmdMemStream << intCmdList[j];
        }

        // wir triggern das senden der serialisierten interrupts
        cmd = firstCompiler.compileOneCmdLineZeroAligned("DSPINTPOST()", &ok);
        cycCmdMemStream << cmd;
    }

    DspVarResolver dspSystemVarResolver;
    DspCmdCompiler dummyCompiler(&dspSystemVarResolver, 0);
    // funktioniert selbst wenn wenn wir keinen mehr haben
    cmd = dummyCompiler.compileOneCmdLineZeroAligned("INVALID()", &ok);
    cycCmdMemStream << cmd; // kommando listen ende
    intCmdMemStream << cmd;

    return true;
}


bool ZDspServer::uploadCommandLists()
{
    flipCommandListSelector();

    if (!writeDspCmdListsToDevNode())
        return false;

    sendCommand2Dsp(QString("DSPCMDPAR,7,%1;").arg(m_currentCmdListSelector));
    return true;
}

void ZDspServer::flipCommandListSelector()
{
    m_currentCmdListSelector = (m_currentCmdListSelector + 1) & 1;
}

bool ZDspServer::writeDspCmdListsToDevNode()
{
    AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
    DspVarResolver dspSystemVarResolver;

    const QString varNameCmdList = m_currentCmdListSelector == 0 ? "CMDLIST" : "ALTCMDLIST";
    ulong offsetCmdList = dspSystemVarResolver.getVarAddress(varNameCmdList) ;
    if(!deviceNode->write(offsetCmdList, m_rawCyclicCmdMem.data(), m_rawCyclicCmdMem.size()))
        return false;

    const QString varNameIntCmdList = m_currentCmdListSelector == 0 ? "INTCMDLIST" : "ALTINTCMDLIST";
    ulong offsetIntCmdList = dspSystemVarResolver.getVarAddress(varNameIntCmdList) ;
    if (!deviceNode->write(offsetIntCmdList, m_rawInterruptCmdMem.data(), m_rawInterruptCmdMem.size()))
        return false;

    return true;
}

QString ZDspServer::loadCmdListAllClients()
{
    QString errs;
    if (!compileCmdListsForAllClientsToRawStream(errs)) {
        qCritical("compileCmdListsForAllClientsToRawStream failed");
        return QString("%1 %2").arg(ZSCPI::scpiAnswer[ZSCPI::errval], errs); // das "fehlerhafte" kommando anhängen
    }
    if (!uploadCommandLists()) {
        qCritical("uploadCommandLists failed");
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    return ZSCPI::scpiAnswer[ZSCPI::ack];
}

static constexpr int dm32DspWorkSpaceBase21362 = 0xE0800;
static constexpr int dm32UserWorkSpaceGlobal21262 = 0x87000;
static constexpr int dm32UserWorkSpaceGlobal21362 = 0x9F000;
static constexpr int dm32DialogWorkSpaceBase21362 = 0xE1800;
static constexpr int dm32UserWorkSpaceBase21362 = 0x98180;
static constexpr int dm32CmdListBase21362 = 0xE2000;
static constexpr int CmdListLen21362 = 3584;
static constexpr int IntCmdListLen21362 = 512;
static constexpr int uwSpaceSize21362 = 32383;

bool ZDspServer::setDspType()
{
    int r = readMagicId();
    if ( r == DeviceNodeDsp::MAGIC_ID21262 ) {
        UserWorkSpaceGlobalSegmentAdr = dm32UserWorkSpaceGlobal21262;
        return m_sDspBootPath.contains("zdsp21262.ldr");
        // adressen im dsp stehen für adsp21262 default richtig
    }
    else if ( r == DeviceNodeDsp::MAGIC_ID21362) {
        UserWorkSpaceGlobalSegmentAdr = dm32UserWorkSpaceGlobal21362;
        if (m_sDspBootPath.contains("zdsp21362.ldr")) {
            // für adsp21362 schreiben wir die adressen um
            dm32DspWorkspace.m_startAddress = dm32DspWorkSpaceBase21362;
            dm32DialogWorkSpace.m_startAddress = dm32DialogWorkSpaceBase21362;
            dm32UserWorkSpace.m_startAddress = dm32UserWorkSpaceBase21362;
            dm32CmdList.m_startAddress = dm32CmdListBase21362;

            TDspVar* pDspVar = &CmdListVar;

            pDspVar->size = IntCmdListLen21362; pDspVar++;
            pDspVar->size = CmdListLen21362; pDspVar++;
            pDspVar->size = IntCmdListLen21362; pDspVar++;
            pDspVar->size = CmdListLen21362;

            pDspVar = &UserWorkSpaceVar;
            pDspVar->size = uwSpaceSize21362;

            return true;
        }
        else
            return false;
    }
    else
        return false;
}

int ZDspServer::readMagicId()
{
    AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
    return deviceNode->dspGetMagicId();
}

bool ZDspServer::Test4HWPresent()
{
    int r = readMagicId();
    return ( (r == DeviceNodeDsp::MAGIC_ID21262) || (r == DeviceNodeDsp::MAGIC_ID21362));
}

bool ZDspServer::Test4DspRunning()
{
    AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
    return deviceNode->dspIsRunning();
}

void ZDspServer::onProtobufClientConnected(VeinTcp::TcpPeer *newClient)
{
    connect(newClient, &VeinTcp::TcpPeer::sigMessageReceived, this, &ZDspServer::onProtobufDataReceived);
    connect(newClient, &VeinTcp::TcpPeer::sigConnectionClosed, this, &ZDspServer::onProtobufDisconnect);
}

void ZDspServer::onProtobufDisconnect(VeinTcp::TcpPeer *peer)
{
    m_zdspClientContainer.delClients(peer);
}

void ZDspServer::onProtobufDataReceived(VeinTcp::TcpPeer *peer, QByteArray message)
{
    executeCommandProto(peer, m_protobufWrapper.byteArrayToProtobuf(message));
}

void ZDspServer::executeCommandProto(VeinTcp::TcpPeer *peer, std::shared_ptr<google::protobuf::Message> cmd)
{
    std::shared_ptr<ProtobufMessage::NetMessage> protobufCommand = std::static_pointer_cast<ProtobufMessage::NetMessage>(cmd);
    if ( (protobufCommand != 0) && (peer != 0)) {
        if (protobufCommand->has_netcommand() && protobufCommand->has_clientid()) {
            // in case of "lost" clients we delete the clients and its data
            QByteArray proxyConnectionId = QByteArray(protobufCommand->clientid().data(), protobufCommand->clientid().size());
            m_zdspClientContainer.delClient(proxyConnectionId);
        }
        else if (protobufCommand->has_clientid() && protobufCommand->has_messagenr()) {
            QByteArray proxyConnectionId = QByteArray(protobufCommand->clientid().data(), protobufCommand->clientid().size());
            quint32 messageNr = protobufCommand->messagenr();
            ProtobufMessage::NetMessage::ScpiCommand scpiCmd = protobufCommand->scpi();

            m_zdspClientContainer.addClient(peer, proxyConnectionId, m_deviceNodeFactory);

            // Stolen from PCBServer::executeCommandProto ---
            QString scpiInput = QString::fromStdString(scpiCmd.command()) +  " " + QString::fromStdString(scpiCmd.parameter());
            ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(scpiInput);
            if(scpiObject) {
                ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
                ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(peer,
                                                                                true,
                                                                                true,
                                                                                proxyConnectionId,
                                                                                messageNr,
                                                                                scpiInput,
                                                                                scpiObject->getType());
                if (scpiDelegate->executeSCPI(protoCmd))
                    emit cmdExecutionDone(protoCmd);
                else {
                    protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
                    emit cmdExecutionDone(protoCmd);
                }
            }
            else {
                ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(peer,
                                                                                true,
                                                                                true,
                                                                                proxyConnectionId,
                                                                                messageNr,
                                                                                scpiInput);
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
                emit cmdExecutionDone(protoCmd);
            }
        }
    }
}

void ZDspServer::onTelnetClientConnected()
{
    qInfo("External SCPI Client connected");
    m_telnetSocket = m_telnetServer->nextPendingConnection();
    connect(m_telnetSocket, &QIODevice::readyRead, this, &ZDspServer::onTelnetDataReceived);
    connect(m_telnetSocket, &QAbstractSocket::disconnected, this, &ZDspServer::onTelnetDisconnect);
}

static const char* telnetClientId = "telnet_client";

void ZDspServer::onTelnetDataReceived()
{
    QString input;
    while ( m_telnetSocket->canReadLine() )
        input += m_telnetSocket->readLine();
    input.remove('\r'); // we remove cr lf
    input.remove('\n');
    qInfo("External SCPI command: %s", qPrintable(input));

    ScpiObjectPtr scpiObject = m_scpiInterface->getSCPIObject(input);
    if(scpiObject) {
        m_zdspClientContainer.addClient(nullptr, telnetClientId, m_deviceNodeFactory);
        ProtonetCommandPtr protoCmd = std::make_shared<ProtonetCommand>(nullptr, false, true, telnetClientId, 0, input);
        ScpiDelegate* scpiDelegate = static_cast<ScpiDelegate*>(scpiObject.get());
        if (!scpiDelegate->executeSCPI(protoCmd))
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
        qInfo("External SCPI response: %s", qPrintable(protoCmd->m_sOutput));
        emit cmdExecutionDone(protoCmd);
    }
}

void ZDspServer::onTelnetDisconnect()
{
    qInfo("External SCPI Client disconnected");
    disconnect(m_telnetSocket, 0, 0, 0); // we disconnect everything
    m_zdspClientContainer.delClient(telnetClientId);
}

void ZDspServer::sendProtoAnswer(ProtonetCommandPtr protoCmd)
{
    CommonScpiMethods::sendProtoAnswer(m_telnetSocket, &m_protobufWrapper, protoCmd);
}
