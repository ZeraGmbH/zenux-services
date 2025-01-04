#include "zdspserver.h"
#include "dspvardevicenodeinout.h"
#include "zdspclient.h"
#include "dsp.h"
#include "dspcmdcompiler.h"
#include "commonscpimethods.h"
#include "pcbserver.h"
#include "devicenodedsp.h"
#include "zscpi_response_definitions.h"
#include <timerfactoryqt.h>
#include <scpinodestaticfunctions.h>
#include <QDebug>
#include <QCoreApplication>
#include <QFinalState>
#include <QDataStream>
#include <QTcpServer>
#include <QTextStream>
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
    m_deviceNodeFactory(deviceNodeFactory),
    m_tcpNetworkFactory(tcpNetworkFactory),
    m_settings(std::move(settings)),
    m_dspInterruptLogStatistics(10000),
    m_pRMConnection(new RMConnection(m_settings->getEthSettings()->getRMIPadr(),
                                     m_settings->getEthSettings()->getPort(EthSettings::resourcemanager),
                                     m_tcpNetworkFactory)),
    m_resourceRegister(m_pRMConnection)
{
    init();
}

void ZDspServer::init()
{
    m_pInitializationMachine = new QStateMachine(this);
    myXMLConfigReader = new Zera::XMLConfig::cReader();

    QState* stateCONF = new QState(); // we start from here
    QFinalState* stateFINISH = new QFinalState(); // and here we finish on any error condition

    stateCONF->addTransition(this, &ZDspServer::abortInit, stateFINISH); // from anywhere we arrive here if some error

    QState* statexmlConfiguration = new QState(stateCONF); // we configure our server with xml file
    QState* statesetupServer = new QState(stateCONF); // we setup our server now
    m_stateconnect2RM = new QState(stateCONF); // we connect to resource manager
    m_stateconnect2RMError = new QState(stateCONF);
    m_stateSendRMIdentAndRegister = new QState(stateCONF); // we send ident. to rm and register our resources

    stateCONF->setInitialState(statexmlConfiguration);

    statexmlConfiguration->addTransition(myXMLConfigReader, &Zera::XMLConfig::cReader::finishedParsingXML, statesetupServer);
    statesetupServer->addTransition(this, &ZDspServer::sigServerIsSetUp, m_stateconnect2RM);

    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);

    QObject::connect(statexmlConfiguration, &QAbstractState::entered, this, &ZDspServer::doConfiguration);
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
    delete m_pDspSettings;
    for (int i = 0; i < m_clientList.count(); i++)
        delete m_clientList.at(i);
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
    if ( pipe(pipeFileDescriptorZdsp1) == -1 ) {
        qCritical("Abort, could not open pipe");
        emit abortInit();
    }
    else {
        fcntl( pipeFileDescriptorZdsp1[1], F_SETFL, O_NONBLOCK);
        fcntl( pipeFileDescriptorZdsp1[0], F_SETFL, O_NONBLOCK);
        m_pNotifier = new QSocketNotifier(pipeFileDescriptorZdsp1[0], QSocketNotifier::Read, this);
        connect(m_pNotifier, &QSocketNotifier::activated, this, &ZDspServer::DspIntHandler);
        ServerParams params = m_settings->getServerParams();
        if (myXMLConfigReader->loadSchema(params.xsdFile)) {
            // we want to initialize all settings first
            m_pDspSettings = new cDSPSettings(myXMLConfigReader);
            connect(myXMLConfigReader,&Zera::XMLConfig::cReader::valueChanged,m_pDspSettings,&cDSPSettings::configXMLInfo);

            if(!myXMLConfigReader->loadXMLFile(params.xmlFile)) {
                qCritical("Abort: Could not open xml file '%s", qPrintable(params.xmlFile));
                emit abortInit();
            }
        }
        else {
            qCritical("Abort: Could not open xsd file '%s", qPrintable(params.xsdFile));
            emit abortInit();
        }
    }
}

void ZDspServer::doSetupServer()
{
    qInfo("Starting doSetupServer");
    initSCPIConnection(QString());
    m_sDspBootPath = m_pDspSettings->getBootFile();
    ActivatedCmdList = 0; // der derzeit aktuelle kommando listen satz (0,1)

    m_protoBufServer = new VeinTcp::TcpServer(m_tcpNetworkFactory, this);
    connect(m_protoBufServer, &VeinTcp::TcpServer::sigClientConnected, this, &ZDspServer::onProtobufClientConnected);

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
            if (m_pDspSettings->isBoot()) { // normally dsp gets booted by dsp server
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
    QCoreApplication::instance()->exit(m_nerror);
}

void ZDspServer::doConnect2RM()
{
    qInfo("Starting doConnect2RM");
    m_nerror = rmConnectionError; // preset error condition
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
    m_protoBufServer->startServer(ethSettings->getPort(EthSettings::protobufserver)); // and can start the server now
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

    // die routinen für das status modell
    scpiGetDeviceLoadAct,
    scpiGetDeviceLoadMax,
    scpiResetDeviceLoadMax,
    scpiGetDspStatus,
    scpiGetDeviceStatus,

    // die routinen für das measure modell
    scpiUnloadCmdList,
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

    addDelegate("STATUS", "DEVICE", SCPI::isQuery, m_scpiInterface, scpiGetDeviceStatus);
    addDelegate("STATUS", "DSP", SCPI::isQuery, m_scpiInterface, scpiGetDspStatus);
    addDelegate("STATUS:DSP:LOAD", "ACTUAL", SCPI::isQuery, m_scpiInterface, scpiGetDeviceLoadAct);
    addDelegate("STATUS:DSP:LOAD", "MAXIMUM", SCPI::isQuery, m_scpiInterface, scpiGetDeviceLoadMax);
    addDelegate("STATUS:DSP:LOAD:MAXIMUM", "RESET", SCPI::isCmdwP, m_scpiInterface, scpiResetDeviceLoadMax);

    connect(this, &ScpiConnection::cmdExecutionDone, this, &ZDspServer::sendProtoAnswer);
}

void ZDspServer::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    int socketNum = m_zdspdClientHash[protoCmd->m_clientId]->getSocket();
    cZDSP1Client* client = GetClient(socketNum);
    DspVarDeviceNodeInOut dspInOut(m_deviceNodeFactory);
    switch (cmdCode)
    {
    case scpiInterfaceRead:
        protoCmd->m_sOutput = handleScpiInterfaceRead(protoCmd->m_sInput);
        break;
    case scpiGetDeviceVersion:
        protoCmd->m_sOutput = getLcaAndDspVersion(client);
        break;
    case scpiGetServerVersion:
        protoCmd->m_sOutput = getServerVersion();
        break;
    case scpiSamplingSystemGetSet:
        if(cmd.isQuery())
            protoCmd->m_sOutput = getSamplingSystemSetup(client);
        else
            protoCmd->m_sOutput = sendCommand2Dsp(QString("DSPCMDPAR,2,%1;").arg(cmd.getParam()));
        break;
    case scpiDspCommandStatGetSet:
        if(cmd.isQuery())
            protoCmd->m_sOutput = getDspCommandStat(client);
        else
            protoCmd->m_sOutput = setDspCommandStat(client, cmd.getParam());
        break;
    case scpiDspMemoryRead:
        protoCmd->m_sOutput = client->readDspVarList(cmd.getParam());
        break;
    case scpiDspMemoryWrite:
        if(dspInOut.doWriteDspVars(cmd.getParam(), &client->m_dspVarResolver))
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
        protoCmd->m_sOutput = loadCmdList(client);
        break;
    case scpiUnloadCmdList:
        protoCmd->m_sOutput = unloadCmdList(client);
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
        protoCmd->m_sOutput = client->readDspVarList("BUSY,1;");  // ab "BUSY"  1 wort lesen
        break;
    case scpiGetDeviceLoadMax:
        protoCmd->m_sOutput = client->readDspVarList("BUSYMAX,1;");  // ab "BUSYMAX"  1 wort lesen
        break;
    case scpiResetDeviceLoadMax:
        if(dspInOut.doWriteDspVars("BUSYMAX,0.0", &client->m_dspVarResolver))
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
        else
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::errexec];
        break;
    case scpiTriggerIntListALL:
        protoCmd->m_sOutput = sendCommand2Dsp(QString("DSPCMDPAR,1;"));
        break;
    case scpiTriggerIntListHKSK:
        protoCmd->m_sOutput = startTriggerIntListHKSK(cmd.getParam(0), socketNum);
        break;
    }
}

QString ZDspServer::handleScpiInterfaceRead(const QString &scpiInput)
{
    // get new SCPI
    QString newScpi = CommonScpiMethods::handleScpiInterfaceRead(m_scpiInterface, scpiInput);

    // add old SCPI
    QDomDocument domDoc("SCPIModel");
    domDoc.setContent(newScpi);

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
                                       .arg(m_pDspSettings->getChannelNr())
                                       .arg(m_pDspSettings->getSamplesSignalPeriod())
                                       .arg(m_pDspSettings->getsamplesMeasurePeriod()));
        if (ret == ZSCPI::scpiAnswer[ZSCPI::ack])
            return true;
        usleep(10000); // give dsp a bit time before next try
    }
    return false;
}

QString ZDspServer::sendCommand2Dsp(QString qs)
{
    // we need a client to do the job
    cZDSP1Client cl(0, 0, m_deviceNodeFactory);
    DspVarDeviceNodeInOut dspInOut(m_deviceNodeFactory);

    int ack;
    if(!cl.readOneDspVarInt("DSPACK", ack))
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    if(ack == InProgress)
        return ZSCPI::scpiAnswer[ZSCPI::busy];
    if(!dspInOut.doWriteDspVars("DSPACK,0;", &cl.m_dspVarResolver) )
        return ZSCPI::scpiAnswer[ZSCPI::errexec]; // reset acknowledge
    if(!dspInOut.doWriteDspVars(qs, &cl.m_dspVarResolver))
        return ZSCPI::scpiAnswer[ZSCPI::errexec];

    AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
    deviceNode->dspRequestInt(); // interrupt beim dsp auslösen
    return ZSCPI::scpiAnswer[ZSCPI::ack]; // sofort fertig melden ....sync. muss die applikation
}

QString ZDspServer::getSamplingSystemSetup(cZDSP1Client* client)
{
    int measmeasmeasChannelCount = 0;
    if (!client->readOneDspVarInt("NCHANNELS", measmeasmeasChannelCount))
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    int samplesPerMeasPeriod = 0;
    if (!client->readOneDspVarInt("NSPERIOD", samplesPerMeasPeriod))
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    int samplesPerSignalPeriod = 0;
    if (!client->readOneDspVarInt("NSMEAS", samplesPerSignalPeriod))
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    return QString("%1,%2,%3").arg(measmeasmeasChannelCount).arg(samplesPerMeasPeriod).arg(samplesPerSignalPeriod);
}

QString ZDspServer::getDspCommandStat(cZDSP1Client* client)
{
    int stat;
    if(!client->readOneDspVarInt("DSPACK", stat))
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    else
        return QString("%1").arg(stat);
}

QString ZDspServer::setDspCommandStat(cZDSP1Client* client, QString scpiParam)
{
    DspVarDeviceNodeInOut dspInOut(m_deviceNodeFactory);
    if(!dspInOut.doWriteDspVars(QString("DSPACK,%1;").arg(scpiParam), &client->m_dspVarResolver) )
        return ZSCPI::scpiAnswer[ZSCPI::errexec];
    else
        return ZSCPI::scpiAnswer[ZSCPI::ack];
}

QString ZDspServer::startTriggerIntListHKSK(QString scpiParam, int socket)
{
    ulong par = scpiParam.toULong();
    par = (par & 0xFFFF )| (socket << 16);
    return sendCommand2Dsp(QString("DSPCMDPAR,4,%1;").arg(par)); // liste mit prozessNr u. HKSK
}

QString ZDspServer::getLcaAndDspVersion(cZDSP1Client* client)
{
    AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
    // LCA
    int rawLcaVersion = deviceNode->lcaRawVersion();
    if ( rawLcaVersion < 0 )
        return ZSCPI::scpiAnswer[ZSCPI::errexec]; // fehler bei der ausführung
    // DSP
    QString p = "VNR,1;";
    p = client->readDspVarList(p);  // ab "VNR"  1 wort lesen
    p = p.section(':',1,1);
    p = p.remove(';');
    double dspVersionFloat = p.toDouble();
    return QString("DSPLCA: V%1.%2;DSP V%3").arg((rawLcaVersion >>8) & 0xff).arg(rawLcaVersion & 0xff).arg(dspVersionFloat, 0, 'f', 2);;
}

QString ZDspServer::getServerVersion()
{
    return QString("%1 %2").arg(ServerName, ServerVersion);;
}

QString ZDspServer::getDspDeviceNode()
{
    return m_settings->getFpgaSettings()->getDspDeviceNode();
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

    if (!m_clientList.isEmpty()) { // wenn vorhanden nutzen wir immer den 1. client zum lesen
        cZDSP1Client *client = m_clientList.first();
        QByteArray ba;
        QString s = "CTRLCMDPAR,20";
        if (client->readOneDspVar(s, &ba)) { // 20 worte lesen
            ulong* pardsp = (ulong*) ba.data();
            int n = pardsp[0]; // anzahl der interrupts
            m_dspInterruptLogStatistics.addValue(n);
            if (n > 20)   // in the moment magic nr
                qInfo ("Number of interrupts in a package: %i exceeds upper limit!", n);
            else {
                for (int i = 1; i < (n+1); i++) {
                    int process = pardsp[i] >> 16;
                    cZDSP1Client *client2 = GetClient(process);
                    if (client2) { // gibts den client noch, der den interrupt haben wollte
                        s = QString("DSPINT:%1").arg(pardsp[i] & 0xFFFF);
                        if (m_clientIDHash.contains(client2)) { // es war ein client der über protobuf (clientid) angelegt wurde
                            ProtobufMessage::NetMessage protobufIntMessage;
                            ProtobufMessage::NetMessage::NetReply *intMessage = protobufIntMessage.mutable_reply();

                            intMessage->set_body(s.toStdString());
                            intMessage->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);

                            QByteArray idba = m_clientIDHash[client2];
                            protobufIntMessage.set_clientid(idba.data(), idba.size() );
                            protobufIntMessage.set_messagenr(0); // interrupt

                            client2->m_pNetClient->sendMessage(m_protobufWrapper.protobufToByteArray(protobufIntMessage));
                        }
                        else {
                            QByteArray block;
                            QDataStream out(&block, QIODevice::WriteOnly);
                            out.setVersion(QDataStream::Qt_4_0);
                            out << (qint32)0;

                            out << s.toUtf8();
                            out.device()->seek(0);
                            out << (qint32)(block.size() - sizeof(qint32));

                            VeinTcp::TcpPeer* pNetclient = client2->m_pNetClient;
                            if (pNetclient == nullptr)
                                m_telnetSocket->write(block);
                            else
                                pNetclient->writeRaw(block);
                        }
                    }
                }
            }
        }
        DspVarDeviceNodeInOut dspInOut(m_deviceNodeFactory);
        dspInOut.doWriteDspVars(QString("CTRLACK,%1;").arg(CmdDone), &client->m_dspVarResolver); // jetzt in jedem fall acknowledge
    }
    else {
        cZDSP1Client dummyClient(0, 0, m_deviceNodeFactory); // dummyClient einrichten
        DspVarDeviceNodeInOut dspInOut(m_deviceNodeFactory);
        dspInOut.doWriteDspVars(QString("CTRLACK,%1;").arg(CmdDone), &dummyClient.m_dspVarResolver); // und rücksetzen
    }
}


bool ZDspServer::BuildDSProgram(QString &errs)
{
    // die programmlisten aller aktiven clients zusammen bauen
    bool ok;
    ulong userMemOffset = dm32UserWorkSpace.StartAdr;

    m_rawCyclicCmdMem.clear();
    m_rawInterruptCmdMem.clear();
    QDataStream cycCmdMemStream(&m_rawCyclicCmdMem, QIODevice::Unbuffered | QIODevice::ReadWrite);
    cycCmdMemStream.setByteOrder(QDataStream::LittleEndian);
    QDataStream intCmdMemStream(&m_rawInterruptCmdMem, QIODevice::Unbuffered | QIODevice::ReadWrite);
    intCmdMemStream.setByteOrder(QDataStream::LittleEndian);

    DspCmdWithParamsRaw cmd;
    if (m_clientList.count() > 0) {
        cZDSP1Client* firstClient = m_clientList.at(0);
        DspCmdCompiler firstCompiler(&firstClient->m_dspVarResolver, firstClient->getSocket());
        cmd = firstCompiler.compileOneCmdLineZeroAligned(QString("DSPMEMOFFSET(%1)").arg(dm32DspWorkspace.StartAdr),
                                                         &ok);
        cycCmdMemStream << cmd;
        for (int i = 0; i < m_clientList.count(); i++) {
            cZDSP1Client* client = m_clientList.at(i);
            if (client->isActive()) {
                DspCmdCompiler compiler(&client->m_dspVarResolver, client->getSocket());
                cmd = compiler.compileOneCmdLineZeroAligned(QString("USERMEMOFFSET(%1)").arg(userMemOffset),
                                                            &ok);
                cycCmdMemStream << cmd;
                intCmdMemStream << cmd;

                if (!client->GenCmdLists(errs, userMemOffset, UserWorkSpaceGlobalSegmentAdr))
                    return false;

                // relokalisieren der daten im dsp
                userMemOffset += client->setStartAdr(userMemOffset, UserWorkSpaceGlobalSegmentAdr);

                QList<DspCmdWithParamsRaw> cycCmdList = client->GetDspCmdList();
                for (int j = 0; j < cycCmdList.size(); j++)
                    cycCmdMemStream << cycCmdList[j];
                QList<DspCmdWithParamsRaw> intCmdList = client->GetDspIntCmdList();
                for (int j = 0; j < intCmdList.size(); j++)
                    intCmdMemStream << intCmdList[j];
            }
        }

        // wir triggern das senden der serialisierten interrupts
        cmd = firstCompiler.compileOneCmdLineZeroAligned("DSPINTPOST()", &ok);
        cycCmdMemStream << cmd;
    }

    cZDSP1Client dummyClient(0, 0, m_deviceNodeFactory); // dummyClient einrichten damit was jetzt kommt noch
    DspCmdCompiler dummyCompiler(&dummyClient.m_dspVarResolver, dummyClient.getSocket());
    // funktioniert selbst wenn wenn wir keinen mehr haben
    cmd = dummyCompiler.compileOneCmdLineZeroAligned("INVALID()", &ok);
    cycCmdMemStream << cmd; // kommando listen ende
    intCmdMemStream << cmd;

    return true;
}


bool ZDspServer::LoadDSProgram()
{
    // die programmlisten aller aktiven clients laden
    QString s,s2;

    ActivatedCmdList = (ActivatedCmdList + 1) & 1;
    if (ActivatedCmdList == 0) {
        s = QString("CMDLIST");
        s2 = QString("INTCMDLIST");
    }
    else {
        s = QString("ALTCMDLIST");
        s2 = QString("ALTINTCMDLIST");
    };

    cZDSP1Client dummyClient(0, 0, m_deviceNodeFactory); // dummyClient einrichten zum laden der kette

    AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
    ulong offset = dummyClient.m_dspVarResolver.getVarAddress(s) ;
    if(!deviceNode->write(offset, m_rawCyclicCmdMem.data(), m_rawCyclicCmdMem.size()))
        return false;

    offset = dummyClient.m_dspVarResolver.getVarAddress(s2) ;
    if (!deviceNode->write(offset, m_rawInterruptCmdMem.data(), m_rawInterruptCmdMem.size()))
        return false;

    // dem dsp die neue liste mitteilen
    sendCommand2Dsp(QString("DSPCMDPAR,7,%1;").arg(ActivatedCmdList));
    return true;
}

QString ZDspServer::loadCmdList(cZDSP1Client* client)
{
    static int count = 0;
    QString errs;
    client->setActive(true);
    QString ret;
    if(BuildDSProgram(errs)) { // die cmdlisten und die variablen waren schlüssig
        if(!LoadDSProgram()) {
            ret = ZSCPI::scpiAnswer[ZSCPI::errexec];
            client->setActive(false);
        }
        else
            ret = ZSCPI::scpiAnswer[ZSCPI::ack];
    }
    else {
        client->setActive(false);
        ret = QString("%1 %2").arg(ZSCPI::scpiAnswer[ZSCPI::errval], errs); // das "fehlerhafte" kommando anhängen
    }
    count++;
    qDebug() << QString("LoadCmdList(%1)").arg(count);
    return ret;
}

QString ZDspServer::unloadCmdList(cZDSP1Client *client)
{
    client->setActive(false);
    QString error;
    BuildDSProgram(error);
    QString ret;
    if (!LoadDSProgram())
        ret = ZSCPI::scpiAnswer[ZSCPI::errexec];
    else
        ret = ZSCPI::scpiAnswer[ZSCPI::ack];
    return ret;
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
            dm32DspWorkspace.StartAdr = dm32DspWorkSpaceBase21362;
            dm32DialogWorkSpace.StartAdr = dm32DialogWorkSpaceBase21362;
            dm32UserWorkSpace.StartAdr = dm32UserWorkSpaceBase21362;
            dm32CmdList.StartAdr = dm32CmdListBase21362;

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

cZDSP1Client* ZDspServer::GetClient(int s)
{
    if (m_clientList.count() > 0) {
        for (int i = 0; i < m_clientList.count(); i++) {
            cZDSP1Client* client = m_clientList.at(i);
            if (client->getSocket() == s)
                return client;
        }
    }
    return nullptr;
}

cZDSP1Client* ZDspServer::GetClient(VeinTcp::TcpPeer *peer)
{
    if (m_clientList.count() > 0) {
        for (int i = 0; i < m_clientList.count(); i++) {
            cZDSP1Client* client = m_clientList.at(i);
            if (client->m_pNetClient == peer)
                return client;
        }
    }
    return nullptr;
}

void ZDspServer::onProtobufClientConnected(VeinTcp::TcpPeer *newClient)
{
    connect(newClient, &VeinTcp::TcpPeer::sigMessageReceived, this, &ZDspServer::onProtobufDataReceived);
    connect(newClient, &VeinTcp::TcpPeer::sigConnectionClosed, this, &ZDspServer::onProtobufDisconnect);
    AddClient(newClient); // we additionally add the client to our list
}

void ZDspServer::onProtobufDisconnect(VeinTcp::TcpPeer *peer)
{
    DelClients(peer);
}

void ZDspServer::onProtobufDataReceived(VeinTcp::TcpPeer *peer, QByteArray message)
{
    executeCommandProto(peer, m_protobufWrapper.byteArrayToProtobuf(message));
}

void ZDspServer::addClientToHash(QByteArray clientId, VeinTcp::TcpPeer *peer)
{
    if (!m_zdspdClientHash.contains(clientId)) { // we didn't get any command from here yet
        cZDSP1Client *zdspclient = AddClient(peer); // we add a new client with the same socket but different identifier
        m_zdspdClientHash[clientId] = zdspclient;
        m_clientIDHash[zdspclient] = clientId; // we need this list in case of interrupts
    }
}

void ZDspServer::executeCommandProto(VeinTcp::TcpPeer *peer, std::shared_ptr<google::protobuf::Message> cmd)
{
    std::shared_ptr<ProtobufMessage::NetMessage> protobufCommand = std::static_pointer_cast<ProtobufMessage::NetMessage>(cmd);
    if ( (protobufCommand != 0) && (peer != 0)) {
        if (protobufCommand->has_netcommand() && protobufCommand->has_clientid()) {
            // in case of "lost" clients we delete the clients and its data
            QByteArray clientId = QByteArray(protobufCommand->clientid().data(), protobufCommand->clientid().size());
            DelClient(clientId);
        }
        else if (protobufCommand->has_clientid() && protobufCommand->has_messagenr()) {
            QByteArray clientId = QByteArray(protobufCommand->clientid().data(), protobufCommand->clientid().size());
            quint32 messageNr = protobufCommand->messagenr();
            ProtobufMessage::NetMessage::ScpiCommand scpiCmd = protobufCommand->scpi();

            addClientToHash(clientId, peer);

            // --- new scpi stolen from PCBServer::executeCommandProto ---
            QString scpiInput = QString::fromStdString(scpiCmd.command()) +  " " + QString::fromStdString(scpiCmd.parameter());
            cSCPIObject* scpiObject = m_scpiInterface->getSCPIObject(scpiInput);
            if(scpiObject) {
                cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
                cProtonetCommand* protoCmd = new cProtonetCommand(peer, true, true, clientId, messageNr, scpiInput, scpiObject->getType());
                if (scpiDelegate->executeSCPI(protoCmd))
                    emit cmdExecutionDone(protoCmd);
                else {
                    protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
                    emit cmdExecutionDone(protoCmd);
                }
            }
            else {
                cProtonetCommand* protoCmd = new cProtonetCommand(peer, true, true, clientId, messageNr, scpiInput);
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
    m_pSCPIClient = AddSCPIClient();
    connect(m_telnetSocket, &QIODevice::readyRead, this, &ZDspServer::onTelnetDataReceived);
    connect(m_telnetSocket, &QAbstractSocket::disconnected, this, &ZDspServer::onTelnetDisconnect);
}

void ZDspServer::onTelnetDataReceived()
{
    QString input;
    while ( m_telnetSocket->canReadLine() )
        input += m_telnetSocket->readLine();
    input.remove('\r'); // we remove cr lf
    input.remove('\n');
    qInfo("External SCPI command: %s", qPrintable(input));

    // New SCPI - stolen fom PCBServer::onTelnetDataReceived()
    cSCPIObject* scpiObject = m_scpiInterface->getSCPIObject(input);
    if(scpiObject) {
        QByteArray clientId = QByteArray(); // we set an empty byte array
        addClientToHash(clientId, nullptr);
        cProtonetCommand* protoCmd = new cProtonetCommand(nullptr, false, true, clientId, 0, input);
        cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
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
    DelSCPIClient();
}

cZDSP1Client* ZDspServer::AddClient(VeinTcp::TcpPeer* netClient)
{
    // fügt einen client hinzu
    m_nSocketIdentifier++;
    if (m_nSocketIdentifier == 0)
        m_nSocketIdentifier++;
    cZDSP1Client* client = new cZDSP1Client(m_nSocketIdentifier, netClient, m_deviceNodeFactory);
    m_clientList.append(client);
    return client;
}

void ZDspServer::DelClients(VeinTcp::TcpPeer* netClient)
{ // entfernt alle cZDSP1Clients die an diesem netClient kleben
    QList<cZDSP1Client*> todeleteList;
    for (int i = 0; i < m_clientList.count(); i++) {
        cZDSP1Client* zdspclient = m_clientList.at(i);
        VeinTcp::TcpPeer* peer = zdspclient->m_pNetClient;
        if (peer == netClient) {
            todeleteList.append(zdspclient);
            if (m_clientIDHash.contains(zdspclient)) {
                QByteArray ba = m_clientIDHash.take(zdspclient);
                m_zdspdClientHash.remove(ba);
            }
        }
    }
    for (int i = 0; i < todeleteList.count(); i++) {
        cZDSP1Client* client = todeleteList.at(i);
        m_clientList.removeOne(client);
        delete client;
    }
    LoadDSProgram(); // after deleting clients we reload dsp program
}

void ZDspServer::DelClient(QByteArray clientId)
{
    if (m_zdspdClientHash.contains(clientId)) {
        cZDSP1Client *client = m_zdspdClientHash.take(clientId);
        m_clientIDHash.remove(client);
        m_clientList.removeOne(client);
        delete client;
        LoadDSProgram(); // after deleting client we reload dsp program ... means unload dsp for this client
    }
}

cZDSP1Client *ZDspServer::AddSCPIClient()
{
    return AddClient(0); // we add this client with netclient (VeinTcp::TcpPeer) = 0 because it is no VeinTcp::TcpPeer but
}

void ZDspServer::DelSCPIClient()
{
    m_clientList.removeAll(m_pSCPIClient);
    LoadDSProgram(); // after deleting client we reload dsp program ... means unload dsp for this client
}

void ZDspServer::sendProtoAnswer(cProtonetCommand *protoCmd)
{
    CommonScpiMethods::sendProtoAnswer(m_telnetSocket, &m_protobufWrapper, protoCmd);
}
