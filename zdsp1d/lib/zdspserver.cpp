// implementation des eigenlichen servers zdsp1(d)

#include "zdspserver.h"
#include "zdspclient.h"
#include "dsp.h"
#include "commonscpimethods.h"
#include "pcbserver.h"
#include "devicenodedsp.h"
#include "scpioldnodestaticfunctions.h"
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

extern cNode* InitCmdTree();

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
    ScpiParserZdsp1d* parser = new(ScpiParserZdsp1d); // das ist der parser
    m_cmdInterpreter = new ScpiCmdInterpreter(this, InitCmdTree(), parser); // das ist der kommando interpreter
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
    QString dspStatus = mGetDspStatus();
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

void ZDspServer::initSCPIConnection(QString leadingNodes)
{
    Q_UNUSED(leadingNodes)
    addDelegate("SYSTEM:INTERFACE", "READ", SCPI::isQuery, m_scpiInterface, scpiInterfaceRead);
    addDelegate("SYSTEM:VERSION", "DEVICE", SCPI::isQuery, m_scpiInterface, scpiGetDeviceVersion);
    addDelegate("SYSTEM:VERSION", "SERVER", SCPI::isQuery, m_scpiInterface, scpiGetServerVersion);
    connect(this, &ScpiConnection::cmdExecutionDone, this, &ZDspServer::sendProtoAnswer);
}

void ZDspServer::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    int socketNum = m_zdspdClientHash[protoCmd->m_clientId]->getSocket();
    cZDSP1Client* client = GetClient(socketNum);
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

    cNodeSCPI* scpiRootNode = static_cast<cNodeSCPI*>(InitCmdTree());
    ScpiOldNodeStaticFunctions::addNodeAndChildrenToXml(scpiRootNode,
                                                        domDoc,
                                                        modelsElem,
                                                        QStringList());
    return domDoc.toString();
}

QString ZDspServer::mTestDsp(QChar* s)
{
    QString par = m_cmdInterpreter->m_pParser->GetKeyword(&s); // holt den parameter aus dem kommando
    bool ok;
    int tmode = par.toInt(&ok);
    int nr = 0;
    bool tstart = false;
    if ((ok) && ( (tmode>=0) && (tmode<2) )) {
        par = m_cmdInterpreter->m_pParser->GetKeyword(&s);
        nr=par.toInt(&ok);
        if ((ok) && ( (nr>=0) && (nr<1000) ))
            tstart = true;
    }
    if (tstart == true) {
        int errcount = 0;
        switch (tmode)
        {
            case 0:
                for (int i=0; i<nr; i++) {
                    mResetDsp(s);
                    int j;
                    for (j=0; j< 100; j++) {
                        usleep(1000);
                        if (Test4DspRunning() == false)
                            break;
                    }
                    if (j==100)
                        errcount++;
                    else {
                        mBootDsp(s);
                        usleep(1000);
                        if (Test4DspRunning() == false)
                            errcount++;
                    }
                    Answer = QString("Test booting dsp %1 times, errors %2").arg(nr).arg(errcount);
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
                cZDSP1Client* cl = GetClient(m_actualSocket);
                QString sadr  = "UWSPACE";
                ulong adr = cl->m_dspVarResolver.adr(sadr) ;
                AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
                for (i=0; i< nr; i++) {
                    if(!deviceNode->write(adr, ba.data(), n*4 )) {
                        Answer = QString("Test write/read dsp data, dev write fault");
                        break; // fehler beim schreiben
                    }
                    if (deviceNode->lseek(adr) < 0) {
                        Answer = QString("Test write/read dsp data, dev seek fault");
                        break; // file positionieren
                    }
                    if (deviceNode->read(ba2.data(), n*4) < 0) {
                        Answer = QString("Test write/read dsp data, dev read fault");
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
                        Answer = QString("Test write/read dsp data, data fault adress %1, write %2, read1 %3, read2 %4").arg(faultadr,16).arg(bw,16).arg(br,16).arg(br2,16);
                        break; // file positionieren
                    }
                }
                if (i==nr)
                    Answer = QString("Test write/read dsp data, %1 times %2 bytes transferred, no errors").arg(nr).arg(n*4);
                break;
        }
    }
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::errval]; // fehler wert
    return Answer;
}

bool ZDspServer::resetDsp()
{
    AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
    bool ok = deviceNode->dspReset();
    if(ok)
        Answer = ZSCPI::scpiAnswer[ZSCPI::ack];
    else {
        qCritical("Error on dsp device reset");
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    return ok;
}

QString ZDspServer::mResetDsp(QChar*)
{
    resetDsp();
    return Answer;
}

bool ZDspServer::bootDsp()
{
    AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
    bool ok = deviceNode->dspBoot(m_sDspBootPath);
    if(ok)
        Answer = ZSCPI::scpiAnswer[ZSCPI::ack];
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    return ok;
}

bool ZDspServer::setSamplingSystem()
{
    for (int i = 0; i < 10; i++) { // we try max. 10 times to set .... this should work
        mCommand2Dsp(QString("DSPCMDPAR,2,%1,%2,%3;").arg(m_pDspSettings->getChannelNr())
                                                     .arg(m_pDspSettings->getSamplesSignalPeriod())
                                                     .arg(m_pDspSettings->getsamplesMeasurePeriod()));
        if (Answer == ZSCPI::scpiAnswer[ZSCPI::ack])
            return true;
        usleep(10000); // give dsp a bit time before next try
    }
    return false;
}

QString ZDspServer::mBootDsp(QChar *)
{
    bootDsp();
    return Answer;
}

QString ZDspServer::mCommand2Dsp(QString qs)
{
    // we need a client to do the job
    cZDSP1Client cl(0, 0, m_deviceNodeFactory);
    do
    {
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
        int ack;
        if (! cl.readDspVarInt("DSPACK", ack))
            break;

        if ( ack ==  InProgress) {
            Answer = ZSCPI::scpiAnswer[ZSCPI::busy];
            break;
        }

        if (! cl.DspVarWrite("DSPACK,0;") )
            break; // reset acknowledge
        if (! cl.DspVarWrite(qs))
            break; // kommando und parameter -> dsp

        AbstractDspDeviceNodePtr deviceNode = m_deviceNodeFactory->getDspDeviceNode();
        deviceNode->dspRequestInt(); // interrupt beim dsp auslösen
        Answer = ZSCPI::scpiAnswer[ZSCPI::ack]; // sofort fertig melden ....sync. muss die applikation

    } while (0);
    return Answer;
}

QString ZDspServer::mSetSamplingSystem(QChar *s)
{
    return mCommand2Dsp(QString("DSPCMDPAR,2,%1;").arg(QString(s)));
}

QString ZDspServer::mSetCommEncryption(QChar *s)
{
    bool ok;
    QString par = m_cmdInterpreter->m_pParser->GetKeyword(&s); // holt den parameter aus dem kommando
    int enc=par.toInt(&ok);
    if ((ok) && ( (enc==0) || (enc==1) )) {
        cZDSP1Client* cl = GetClient(m_actualSocket);
        cl->SetEncryption(enc);
        Answer = ZSCPI::scpiAnswer[ZSCPI::ack]; // acknowledge
    }
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::errval]; // fehler wert

    return Answer;
}

QString ZDspServer::mGetSamplingSystem()
{
    do
    {
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
        int n, ss, sm;

        cZDSP1Client* cl = GetClient(m_actualSocket);

        if (!cl->readDspVarInt("NCHANNELS", n))
            break;
        if (!cl->readDspVarInt("NSPERIOD", ss))
            break;
        if (!cl->readDspVarInt("NSMEAS", sm))
            break;

        Answer = QString("%1,%2,%3").arg(n).arg(ss).arg(sm);
    } while (0);

    return Answer;
}

QString ZDspServer::mGetCommEncryption()
{
    cZDSP1Client* cl = GetClient(m_actualSocket);
    Answer = QString::number(cl->GetEncryption());

    return Answer;
}

QString ZDspServer::mSetDspCommandStat(QChar *s)
{
    Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    QString ss;
    cZDSP1Client* cl = GetClient(m_actualSocket);
    if (! cl->DspVarWrite(ss = QString("DSPACK,%1;").arg(QString(s))) )
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::ack];
    return Answer;
}

QString ZDspServer::mGetDspCommandStat()
{
    int stat;
    cZDSP1Client* cl = GetClient(m_actualSocket);
    if (!cl->readDspVarInt("DSPACK", stat))
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    else
        Answer = QString("%1").arg(stat);
    return Answer;
}

QString ZDspServer::mTriggerIntListHKSK(QChar *s)
{
    QString ss(s);
    ulong par = ss.toULong();
    par = (par & 0xFFFF )| (m_actualSocket << 16);
    return mCommand2Dsp(QString("DSPCMDPAR,4,%1;").arg(par)); // liste mit prozessNr u. HKSK
}

QString ZDspServer::mTriggerIntListALL(QChar *)
{
    return mCommand2Dsp(QString("DSPCMDPAR,1;"));
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
    p = client->DspVarListRead(p);  // ab "VNR"  1 wort lesen
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

QString ZDspServer::mGetDspStatus()
{
    if ( Test4DspRunning() )
        Answer = dsprunning;
    else
        Answer = dspnrunning;
    return Answer;
}

QString ZDspServer::mGetDeviceStatus()
{
    if ( Test4HWPresent() )
        Answer = devavail;
    else
        Answer = devnavail;
    return Answer;
}

QString ZDspServer::mGetDeviceLoadAct()
{
    cZDSP1Client* cl = GetClient(m_actualSocket);
    QString p = "BUSY,1;";
    Answer = cl->DspVarListRead(p);  // ab "BUSY"  1 wort lesen
    return Answer;
}

QString ZDspServer::mGetDeviceLoadMax()
{
    cZDSP1Client* cl = GetClient(m_actualSocket);
    QString p = "BUSYMAX,1;";
    Answer = cl->DspVarListRead(p);  // ab "BUSYMAX"  1 wort lesen
    return Answer;
}

QString ZDspServer::mResetDeviceLoadMax()
{
    cZDSP1Client* cl = GetClient(m_actualSocket);
    QString p = "BUSYMAX,0.0";
    Answer = cl->DspVarWriteRM(p);
    return Answer;
}

QDataStream& operator<<(QDataStream& ds,cDspCmd c)
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
        if (client->DspVarRead(s, &ba)) { // 20 worte lesen
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
        client->DspVarWrite(s = QString("CTRLACK,%1;").arg(CmdDone)); // jetzt in jedem fall acknowledge
    }
    else {
        cZDSP1Client dummyClient(0, 0, m_deviceNodeFactory); // dummyClient einrichten
        QString s = QString("CTRLACK,%1;").arg(CmdDone);
        dummyClient.DspVarWrite(s); // und rücksetzen
    }
}


bool ZDspServer::BuildDSProgram(QString &errs)
{
    // die programmlisten aller aktiven clients zusammen bauen
    bool ok;
    ulong userMemoryOffset = dm32UserWorkSpace.StartAdr;

    CmdMem.clear();
    CmdIntMem.clear();
    QDataStream mds1 ( &CmdMem, QIODevice::Unbuffered | QIODevice::ReadWrite );
    mds1.setByteOrder(QDataStream::LittleEndian);
    QDataStream mds2 ( &CmdIntMem, QIODevice::Unbuffered | QIODevice::ReadWrite );
    mds2.setByteOrder(QDataStream::LittleEndian);

    cDspCmd cmd;
    QString s;

    if (m_clientList.count() > 0) {
        cZDSP1Client* client = m_clientList.at(0);
        s =  QString( "DSPMEMOFFSET(%1)" ).arg(dm32DspWorkspace.StartAdr);
        cmd = client->GenDspCmd(s, &ok, 0, 0);
        mds1 << cmd;
        for (int i = 0; i < m_clientList.count(); i++) {
            client = m_clientList.at(i);
            if (client->isActive()) {
                s =  QString( "USERMEMOFFSET(%1)" ).arg(userMemoryOffset);
                cmd = client->GenDspCmd(s, &ok, 0, 0);
                mds1 << cmd;
                mds2 << cmd;

                if (!client->GenCmdLists(errs, userMemoryOffset, UserWorkSpaceGlobalSegmentAdr))
                    return false;

                userMemoryOffset += client->setStartAdr(userMemoryOffset, UserWorkSpaceGlobalSegmentAdr); // relokalisieren der daten im dsp

                QList<cDspCmd> cmdl = client->GetDspCmdList();
                for (int j = 0; j < cmdl.size(); j++ ) mds1 << cmdl[j]; // cycl. liste
                QList<cDspCmd> cmdl2 = client->GetDspIntCmdList();
                for ( int j = 0; j < cmdl2.size(); j++ ) mds2 << cmdl2[j]; // interrupt liste
            }
        }

        client = m_clientList.at(0);
        s = QString( "DSPINTPOST()"); // wir triggern das senden der serialisierten interrupts
        cmd = client->GenDspCmd(s, &ok, 0, 0);
        mds1 << cmd;
    }

    cZDSP1Client dummyClient(0, 0, m_deviceNodeFactory); // dummyClient einrichten damit was jetzt kommt noch
    s =  QString( "INVALID()"); // funktioniert selbst wenn wenn wir keinen mehr haben
    cmd = dummyClient.GenDspCmd(s, &ok, 0, 0);
    mds1 << cmd; // kommando listen ende
    mds2 << cmd;

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
    ulong offset = dummyClient.m_dspVarResolver.adr(s) ;
    if(!deviceNode->write(offset, CmdMem.data(), CmdMem.size()))
        return false;

    offset = dummyClient.m_dspVarResolver.adr(s2) ;
    if (!deviceNode->write(offset, CmdIntMem.data(), CmdIntMem.size()))
        return false;

    // dem dsp die neue liste mitteilen
    mCommand2Dsp(QString("DSPCMDPAR,7,%1;").arg(ActivatedCmdList));
    return true;
}

QString ZDspServer::mUnloadCmdList(QChar *)
{
    QString error;
    cZDSP1Client* cl = GetClient(m_actualSocket);
    cl->SetActive(false);
    BuildDSProgram(error); // wir bauen neu
    if (!LoadDSProgram()) // und laden
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::ack];
    return Answer;
}

QString ZDspServer::mLoadCmdList(QChar *)
{
    static int count = 0;
    cZDSP1Client* cl = GetClient(m_actualSocket);
    QString errs;
    cl->SetActive(true);
    if (BuildDSProgram(errs)) { // die cmdlisten und die variablen waren schlüssig
        if (!LoadDSProgram()) {
            Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
            cl->SetActive(false);
        }
        else
            Answer = ZSCPI::scpiAnswer[ZSCPI::ack];
    }
    else {
        cl->SetActive(false);
        Answer = QString("%1 %2").arg(ZSCPI::scpiAnswer[ZSCPI::errval], errs); // das "fehlerhafte" kommando anhängen
    }
    count++;
    qDebug() << QString("LoadCmdList(%1)").arg(count);
    return Answer;
}

QString ZDspServer::setRawActualValueList(QChar *s)
{
    QString qs(s);
    cZDSP1Client* cl = GetClient(m_actualSocket);
    Answer  = cl->setRawActualValueList(qs);
    return Answer;
}

QString ZDspServer::getRawActualValueList()
{
    cZDSP1Client* cl = GetClient(m_actualSocket);
    Answer = cl->getRawActualValueList();
    return Answer;
}

QString ZDspServer::mSetCmdIntList(QChar *s)
{
    QString par(s);
    cZDSP1Client* cl = GetClient(m_actualSocket);
    Answer = cl->SetCmdIntListDef(par);
    return Answer;
}

QString ZDspServer::mGetCmdIntList()
{
    cZDSP1Client* cl = GetClient(m_actualSocket);
    Answer = cl->GetCmdIntListDef();
    return Answer;
}

QString ZDspServer::mSetCmdList(QChar *s)
{
    QString par(s);
    cZDSP1Client* cl = GetClient(m_actualSocket);
    Answer = cl->SetCmdListDef(par);
    return Answer;
}

QString ZDspServer::mGetCmdList()
{
    cZDSP1Client* cl = GetClient(m_actualSocket);
    Answer = cl->GetCmdListDef();
    return Answer;
}


QString ZDspServer::mMeasure(QChar *s)
{
    QString par(s); // holt den parameter aus dem kommando
    cZDSP1Client* cl = GetClient(m_actualSocket);
    Answer = cl->readActValues(par);
    return Answer;
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

QString ZDspServer::mDspMemoryRead(QChar* s)
{
    QString par(s);
    cZDSP1Client* cl = GetClient(m_actualSocket);
    Answer = cl->DspVarListRead(par);
    return Answer;
}

QString ZDspServer::mDspMemoryWrite(QChar* s)
{
    QString par(s);
    cZDSP1Client* cl = GetClient(m_actualSocket);
    Answer = cl->DspVarWriteRM(par);
    return Answer;
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

            if (!m_zdspdClientHash.contains(clientId)) { // we didn't get any command from here yet
                cZDSP1Client *zdspclient = AddClient(peer); // we add a new client with the same socket but different identifier
                m_zdspdClientHash[clientId] = zdspclient;
                m_clientIDHash[zdspclient] = clientId; // we need this list in case of interrupts
            }

            // --- new scpi stolen from PCBServer::executeCommandProto ---
            QString scpiInput = QString::fromStdString(scpiCmd.command()) +  " " + QString::fromStdString(scpiCmd.parameter());
            cSCPIObject* scpiObject =  m_scpiInterface->getSCPIObject(scpiInput);
            if (scpiObject) {
                cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
                cProtonetCommand* protoCmd = new cProtonetCommand(peer, true, true, clientId, messageNr, scpiInput, scpiObject->getType());
                if (scpiDelegate->executeSCPI(protoCmd))
                    emit cmdExecutionDone(protoCmd);
                else {
                    protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
                    emit cmdExecutionDone(protoCmd);
                }
                return; // for now
            }
            // As long as old scpi is around nak is not a good idea
            /*else {
                protoCmd = new cProtonetCommand(peer, true, true, clientId, messageNr, scpiInput);
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
                emit cmdExecutionDone(protoCmd);
            }*/

            // Old SCPI
            m_actualSocket = m_zdspdClientHash[clientId]->getSocket(); // we set the actual socket (identifier) we have to work on
            QString input = QString::fromStdString(scpiCmd.command()) +  " " + QString::fromStdString(scpiCmd.parameter());
            QString output = m_cmdInterpreter->CmdExecute(input);

            ProtobufMessage::NetMessage protobufAnswer;
            ProtobufMessage::NetMessage::NetReply *Answer = protobufAnswer.mutable_reply();

            // dependent on rtype caller can se ack, nak, error
            // in case of error the body has to be analysed for details
            if (output.contains(ZSCPI::scpiAnswer[ZSCPI::ack]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);
            else
            if (output.contains(ZSCPI::scpiAnswer[ZSCPI::nak]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_NACK);
            else
            if (output.contains(ZSCPI::scpiAnswer[ZSCPI::busy]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (output.contains(ZSCPI::scpiAnswer[ZSCPI::errval]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (output.contains(ZSCPI::scpiAnswer[ZSCPI::errexec]))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);

            QByteArray ba = output.toUtf8();
            Answer->set_body(ba.data()); // in any case we set the body
            protobufAnswer.set_clientid(clientId, clientId.count());
            protobufAnswer.set_messagenr(messageNr);
            peer->sendMessage(m_protobufWrapper.protobufToByteArray(protobufAnswer));
        }
        else {
            m_actualSocket = GetClient(peer)->getSocket();

            QString input =  QString::fromStdString(protobufCommand->scpi().command());
            QString output = m_cmdInterpreter->CmdExecute(input);

            QByteArray block;
            QDataStream out(&block, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_0);
            out << (qint32)0;

            out << output.toUtf8();
            out.device()->seek(0);
            out << (qint32)(block.size() - sizeof(qint32));

            peer->writeRaw(block);
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
    QString output = m_cmdInterpreter->CmdExecute(input) + "\n";
    QByteArray ba = output.toLatin1();
    m_telnetSocket->write(ba);
    qInfo("External SCPI response: %s", qPrintable(output));
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

QString ZDspServer::SCPICmd(SCPICmdType cmd, QChar *s)
{
    switch ((int)cmd)
    {
    case    TestDsp:            return mTestDsp(s);
    case 	SetRavList: 		return setRawActualValueList(s);
    case 	SetCmdList: 		return mSetCmdList(s);
    case   SetCmdIntList: 		return mSetCmdIntList(s);
    case 	Measure:            return mMeasure(s);
    case 	UnloadCmdList: 		return mUnloadCmdList(s);
    case 	LoadCmdList: 		return mLoadCmdList(s);
    case 	DspMemoryRead: 		return mDspMemoryRead(s);
    case   DspMemoryWrite:		return mDspMemoryWrite(s);
    case   SetSamplingSystem:	return mSetSamplingSystem(s);
    case	SetCommEncryption:	return mSetCommEncryption(s);
    case   SetDspCommandStat:	return mSetDspCommandStat(s);
    case   TriggerIntListHKSK:	return mTriggerIntListHKSK(s);
    case   TriggerIntListALL:		return mTriggerIntListALL(s);
    case   ResetDeviceLoadMax:	return mResetDeviceLoadMax();
    }
    Answer = "ProgrammierFehler"; // hier sollten wir nie hinkommen

    return Answer;
}

QString ZDspServer::SCPIQuery(SCPICmdType cmdEnum)
{
    switch ((int)cmdEnum)
    {
    case		GetDeviceLoadMax: 	return mGetDeviceLoadMax();
    case 		GetDeviceLoadAct: 	return mGetDeviceLoadAct();
    case		GetDspStatus:		return mGetDspStatus();
    case 		GetDeviceStatus: 		return mGetDeviceStatus();
    case 		GetRavList: 		return getRawActualValueList();
    case 		GetCmdIntList: 		return mGetCmdIntList();
    case 		GetCmdList: 		return mGetCmdList();
    case		GetSamplingSystem:	return mGetSamplingSystem();
    case        GetCommEncryption:	return mGetCommEncryption();
    case 		GetDspCommandStat:	return mGetDspCommandStat();
    }
    Answer = "ProgrammierFehler"; // hier sollten wir nie hinkommen
    return Answer;
}

void ZDspServer::sendProtoAnswer(cProtonetCommand *protoCmd)
{
    CommonScpiMethods::sendProtoAnswer(m_telnetSocket, &m_protobufWrapper, protoCmd);
}
