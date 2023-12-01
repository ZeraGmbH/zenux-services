// implementation des eigenlichen servers zdsp1(d)

#include "zdspclient.h"
#include "zdspserver.h"
#include "dsp.h"
#include "dspvarparser.h"
#include "scpi-zdsp.h"
#include "pcbserver.h"
#include "dspdevicenode.h"
#include "zscpi_response_definitions.h"
#include <QDebug>
#include <QCoreApplication>
#include <QFinalState>
#include <QDataStream>
#include <QTcpServer>
#include <QTextStream>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#ifdef SYSTEMD_NOTIFICATION
#include <systemd/sd-daemon.h>
#endif

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


static char devavail[6] = "avail";
static char devnavail[10]= "not avail";

static char dsprunning[8] = "running";
static char dspnrunning[12]= "not running";


int pipeFD[2];
void SigHandler(int)
{
    const static char pipeFDBuf[2] = "I";
    write(pipeFD[1], pipeFDBuf, 1);
}


struct sigaction mySigAction;
// sigset_t mySigmask, origSigmask;


ServerParams ZDspServer::defaultParams {ServerName, ServerVersion, "/etc/zera/zdsp1d/zdsp1d.xsd", "/etc/zera/zdsp1d/zdsp1d.xml"};

ZDspServer::ZDspServer(DspDeviceNodeInterfaceUPtr dspDevNode, ServerParams params) :
    m_params(params),
    m_dspDevNode(std::move(dspDevNode))
{
    m_pInitializationMachine = new QStateMachine(this);
    myXMLConfigReader = new Zera::XMLConfig::cReader();

    QState* stateCONF = new QState(); // we start from here
    QFinalState* stateFINISH = new QFinalState(); // and here we finish on any error condition

    stateCONF->addTransition(this, SIGNAL(abortInit()),stateFINISH); // from anywhere we arrive here if some error

    QState* statexmlConfiguration = new QState(stateCONF); // we configure our server with xml file
    QState* statesetupServer = new QState(stateCONF); // we setup our server now
    m_stateconnect2RM = new QState(stateCONF); // we connect to resource manager
    m_stateconnect2RMError = new QState(stateCONF);
    m_stateSendRMIdentAndRegister = new QState(stateCONF); // we send ident. to rm and register our resources

    stateCONF->setInitialState(statexmlConfiguration);

    statexmlConfiguration->addTransition(myXMLConfigReader, SIGNAL(finishedParsingXML(bool)), statesetupServer);
    statesetupServer->addTransition(this, SIGNAL(sigServerIsSetUp()), m_stateconnect2RM);

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
}

ZDspServer::~ZDspServer()
{
    delete m_pDebugSettings;
    delete  m_pETHSettings;
    delete m_pDspSettings;
    for (int i = 0; i < clientlist.count(); i++)
        delete clientlist.at(i);
    delete m_pRMConnection;
    delete m_pSCPIServer;
    resetDsp(); // we reset the dsp when we close the server
    m_dspDevNode->close();
    close(pipeFD[0]);
    close(pipeFD[1]);
}

void ZDspServer::doConfiguration()
{
    if ( pipe(pipeFD) == -1 ) {
        qCritical("Abort, could not open pipe");
        emit abortInit();
    }
    else {
        fcntl( pipeFD[1], F_SETFL, O_NONBLOCK);
        fcntl( pipeFD[0], F_SETFL, O_NONBLOCK);
        m_pNotifier = new QSocketNotifier(pipeFD[0], QSocketNotifier::Read, this);
        connect(m_pNotifier, &QSocketNotifier::activated, this, &ZDspServer::DspIntHandler);
        if (myXMLConfigReader->loadSchema(m_params.xsdFile)) {
            // we want to initialize all settings first
            m_pDebugSettings = new cDebugSettings(myXMLConfigReader);
            connect(myXMLConfigReader,&Zera::XMLConfig::cReader::valueChanged,m_pDebugSettings,&cDebugSettings::configXMLInfo);
            m_pETHSettings = new EthSettings(myXMLConfigReader);
            connect(myXMLConfigReader,&Zera::XMLConfig::cReader::valueChanged,m_pETHSettings,&EthSettings::configXMLInfo);
            m_pDspSettings = new cDSPSettings(myXMLConfigReader);
            connect(myXMLConfigReader,&Zera::XMLConfig::cReader::valueChanged,m_pDspSettings,&cDSPSettings::configXMLInfo);

            if(!myXMLConfigReader->loadXMLFile(m_params.xmlFile)) {
                qCritical("Abort: Could not open xml file '%s", qPrintable(m_params.xmlFile));
                emit abortInit();
            }
        }
        else {
            qCritical("Abort: Could not open xsd file '%s", qPrintable(m_params.xsdFile));
            emit abortInit();
        }
    }
}


void ZDspServer::doSetupServer()
{
    DspVarParser* parser = new(DspVarParser); // das ist der parser
    m_cmdInterpreter = new ScpiCmdInterpreter(this, InitCmdTree(), parser); // das ist der kommando interpreter
    m_sDspSerialNumber = "Unknown"; // kennen wir erst mal nicht
    m_sDspBootPath = m_pDspSettings->getBootFile();
    ActivatedCmdList = 0; // der derzeit aktuelle kommando listen satz (0,1)

    myProtonetServer =  new XiQNetServer(this);
    myProtonetServer->setDefaultWrapper(&m_ProtobufWrapper);
    connect(myProtonetServer, &XiQNetServer::sigClientConnected, this, &ZDspServer::onEstablishNewConnection);
    myProtonetServer->startServer(m_pETHSettings->getPort(EthSettings::protobufserver)); // and can start the server now

    if (m_pETHSettings->isSCPIactive()) {
        m_pSCPIServer = new QTcpServer(this);
        m_pSCPIServer->setMaxPendingConnections(1); // we only accept 1 client to connect
        connect(m_pSCPIServer, &QTcpServer::newConnection, this, &ZDspServer::setSCPIConnection);
        m_pSCPIServer->listen(QHostAddress::AnyIPv4, m_pETHSettings->getPort(EthSettings::scpiserver));
    }
    m_sDspDeviceNode = m_pDspSettings->getDeviceNode(); // we try to open the dsp device
    if (DspDevOpen() < 0) {
        qCritical("Abort: Could not open dsp device node '%s", qPrintable(m_sDspDeviceNode));
        emit abortInit();
    }
    else {
        mySigAction.sa_handler = &SigHandler; // signal handler einrichten
        sigemptyset(&mySigAction.sa_mask);
        mySigAction. sa_flags = SA_RESTART;
        mySigAction.sa_restorer = NULL;
        sigaction(SIGIO, &mySigAction, NULL); // handler für sigio definieren
        m_dspDevNode->enableFasync();
        m_retryRMConnect = 100;
        m_retryTimer.setSingleShot(true);
        connect(&m_retryTimer, &QTimer::timeout, this, &ZDspServer::sigServerIsSetUp);

        if (setDspType()) { // interrogate the mounted dsp device type and bootfile match
            if (m_pDspSettings->isBoot()) { // normally dsp gets booted by dsp server
                if (resetDsp() && bootDsp()) { // and try to reset and then boot it
                    if (setSamplingSystem()) { // now we try to set the dsp's sampling system
                        // our resource manager connection must be opened after configuration is done
                        m_pRMConnection = new RMConnection(m_pETHSettings->getRMIPadr(), m_pETHSettings->getPort(EthSettings::resourcemanager));
                        m_stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connected()), m_stateSendRMIdentAndRegister);
                        m_stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connectionRMError()), m_stateconnect2RMError);
                        m_stateconnect2RMError->addTransition(this, SIGNAL(sigServerIsSetUp()), m_stateconnect2RM);
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
                m_pRMConnection = new RMConnection(m_pETHSettings->getRMIPadr(), m_pETHSettings->getPort(EthSettings::resourcemanager));
                m_stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connected()), m_stateSendRMIdentAndRegister);
                m_stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connectionRMError()), m_stateconnect2RMError);
                m_stateconnect2RMError->addTransition(this, SIGNAL(sigServerIsSetUp()), m_stateconnect2RM);
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
    m_nerror = rmConnectionError; // preset error condition
    m_pRMConnection->connect2RM();
}

void ZDspServer::connect2RMError()
{
    m_retryRMConnect--;
    if (m_retryRMConnect == 0)
        emit abortInit();
    else
        m_retryTimer.start(200);
}

void ZDspServer::doIdentAndRegister()
{
    m_pRMConnection->SendIdent(ServerName);

    quint32 port = m_pETHSettings->getPort(EthSettings::protobufserver);

    QString cmd, par;

    m_pRMConnection->SendCommand(cmd = QString("RESOURCE:ADD"), par = QString("DSP;DSP1;;ADSP Signal Processor;%1;")
                                 .arg(port));

    TDspVar* pDspVar = &CmdListVar;
    m_pRMConnection->SendCommand(cmd = QString("RESOURCE:ADD"), par = QString("DSP1;PGRMEMI;%1;DSP ProgramMemory(Interrupt);%2;")
                                 .arg(pDspVar->size)
                                 .arg(port));
    pDspVar++;

    m_pRMConnection->SendCommand(cmd = QString("RESOURCE:ADD"), par = QString("DSP1;PGRMEMC;%1;DSP ProgramMemory(Cyclic);%2;")
                                 .arg(pDspVar->size)
                                 .arg(port));

    pDspVar = &UserWorkSpaceVar;
    m_pRMConnection->SendCommand(cmd = QString("RESOURCE:ADD"), par = QString("DSP1;USERMEM;%1;DSP UserMemory;%2;")
                                 .arg(pDspVar->size)
                                 .arg(port));
#ifdef SYSTEMD_NOTIFICATION
    sd_notify(0, "READY=1");
#endif
}

int ZDspServer::DspDevOpen()
{
    int descriptor = m_dspDevNode->open(m_sDspDeviceNode.toLatin1().data());
    if (descriptor  < 0 )
        qWarning("Error opening dsp device: %s", qPrintable(m_sDspDeviceNode));
    return descriptor;
}

int ZDspServer::DspDevSeek(ulong adr)
{
    int r = m_dspDevNode->lseek(adr);
    if (r < 0)
        qWarning("Error positioning dsp device: %s", qPrintable(m_sDspDeviceNode));
    return r;
}

int ZDspServer::DspDevWrite(char* buf, int len)
{
    int r = m_dspDevNode->write(buf, len);
    if (r <0 )
        qWarning("Error writing dsp device: %s", qPrintable(m_sDspDeviceNode));
    return r;
}

int ZDspServer::DspDevRead(char* buf, int len)
{
    int r = m_dspDevNode->read(buf, len);
    if (r < 0 )
        qWarning("Error reading dsp device: %s", qPrintable(m_sDspDeviceNode));
    return r;
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
                for (i=0; i< nr; i++) {
                    if (DspDevSeek(adr) < 0) {
                        Answer = QString("Test write/read dsp data, dev seek fault");
                        break; // file positionieren
                    }
                    if (DspDevWrite(ba.data(), n*4 ) < 0) {
                        Answer = QString("Test write/read dsp data, dev write fault");
                        break; // fehler beim schreiben
                    }
                    if (DspDevSeek(adr) < 0) {
                        Answer = QString("Test write/read dsp data, dev seek fault");
                        break; // file positionieren
                    }
                    if (DspDevRead(ba2.data(), n*4) < 0) {
                        Answer = QString("Test write/read dsp data, dev read fault");
                        break; // fehler beim schreiben
                    }
                    bool err = false;
                    for (j=0; j<n*4; j++) {
                        if (ba[j] != ba2[j]) {
                            bw = ba[j]; // das geschriebene byte
                            br = ba2[j]; // das gelesene byte
                            faultadr = adr + j;
                            DspDevRead(ba2.data(), n*4);
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
    bool ok = m_dspDevNode->dspReset();
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
    bool ok = m_dspDevNode->dspBoot(m_sDspBootPath);
    if(ok)
        Answer = ZSCPI::scpiAnswer[ZSCPI::ack];
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    return ok;
}

bool ZDspServer::setSamplingSystem()
{
    QString ss;
    for (int i = 0; i < 10; i++) // we try max. 10 times to set .... this should work
    {
        mCommand2Dsp(ss = QString("DSPCMDPAR,2,%1,%2,%3;").arg(m_pDspSettings->getChannelNr())
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

QString ZDspServer::mGetPCBSerialNumber()
{
    return m_sDspSerialNumber;
}

QString ZDspServer::mCommand2Dsp(QString& qs)
{
    // we need a client to do the job
    cZDSP1Client cl(0, 0, this);
    do
    {
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
        int ack;

        QString ss;
        if (! cl.DspVar( ss ="DSPACK",ack)) break;

        if ( ack ==  InProgress)
        {
            Answer = ZSCPI::scpiAnswer[ZSCPI::busy];
            break;
        }

        if (! cl.DspVarWrite(ss = "DSPACK,0;") ) break; // reset acknowledge
        if (! cl.DspVarWrite(qs)) break; // kommando und parameter -> dsp

        m_dspDevNode->dspRequestInt(); // interrupt beim dsp auslösen
        Answer = ZSCPI::scpiAnswer[ZSCPI::ack]; // sofort fertig melden ....sync. muss die applikation

    } while (0);
    return Answer;
}

QString ZDspServer::mSetSamplingSystem(QChar *s)
{
    QString ss;

    return mCommand2Dsp(ss = QString("DSPCMDPAR,2,%1;").arg(QString(s)));
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

        QString s;
        if (! cl->DspVar(s = "NCHANNELS",n)) break;
        if (! cl->DspVar(s = "NSPERIOD",ss)) break;
        if (! cl->DspVar(s = "NSMEAS",sm)) break;

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

QString ZDspServer::mSetEN61850SourceAdr(QChar* s)
{
    int i;
    QByteArray ba;
    QString ss(s);
    ushort adr[6];
    bool ok;
    for (i = 0;i < 6;i++)
    {
        QString t = ss.section(',',i,i); // versuch 6 voneinander mit , getrennte parameter zu lesen
        adr[i] = t.toUShort(&ok);
        if (ok) ok &= (adr[i] < 256); // test ob adr bytes < 256
        if (!ok) break;
    }
    if (!ok)
        Answer = ZSCPI::scpiAnswer[ZSCPI::nak];
    else
        do
    {
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec]; // vorbesetzen
        QString as;
        cZDSP1Client* cl = GetClient(m_actualSocket);
        if (!cl->DspVarRead(as = "ETHDESTSOURCEADRESS,3", &ba)) break;
        else
        {
            ulong* pardsp = (ulong*) ba.data();
            pardsp[1] &= 0xFFFF0000; // die oberen bits behalten wir weil dest adr
            pardsp[1] = pardsp[1] | (adr[0] << 8) | adr[1];
            pardsp[2] = 0;
            for (i = 2;i<6;i++) pardsp[2] = (pardsp[2] << 8) + adr[i];
            mCommand2Dsp(as = QString("DSPCMDPAR,6,%1,%2,%3;").arg(pardsp[0]).arg(pardsp[1]).arg(pardsp[2])); // setzt answer schon
        }
    } while(0);

    return Answer;
}

QString ZDspServer::mSetEN61850DestAdr(QChar *s)
{
    int i;
    QString ss(s);
    ushort adr[6]; // 2 * 4 werte reservieren
    bool ok;
    for (i = 0;i < 6;i++) {
        QString t = ss.section(',',i,i); // versuch 6 voneinander mit , getrennte parameter zu lesen
        adr[i] = t.toUShort(&ok);
        if (ok) ok &= (adr[i] < 256); // test ob adr bytes < 256
        if (!ok) break;
    }
    if (!ok)
        Answer = ZSCPI::scpiAnswer[ZSCPI::nak];
    else
        do {
            Answer = ZSCPI::scpiAnswer[ZSCPI::errexec]; // vorbesetzen
            cZDSP1Client* cl = GetClient(m_actualSocket);
            QString as;
            QByteArray ba;
            if (!cl->DspVarRead(as = "ETHDESTSOURCEADRESS,3", &ba))
                break;
            else {
                ulong* pardsp = (ulong*) ba.data();
                pardsp[0] = 0;
                for (i = 0;i<4;i++) pardsp[0] = (pardsp[0] << 8) +adr[i];
                pardsp[1] &= 0xFFFF; // die unteren bits behalten wir weil source adr
                pardsp[1] = pardsp[1] | (adr[4] << 24) | (adr[5] << 16);
                mCommand2Dsp(as = QString("DSPCMDPAR,6,%1,%2,%3;").arg(pardsp[0]).arg(pardsp[1]).arg(pardsp[2]));
            }
        } while(0);
    return Answer;
}

QString ZDspServer::mSetEN61850EthTypeAppId(QChar *s)
{
    QString ss;
    cZDSP1Client* cl = GetClient(m_actualSocket);
    if (! cl->DspVarWrite(ss = QString("ETHTYPEAPPID,%1;").arg(QString(s))) )
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::ack];
    return Answer;
}

QString ZDspServer::mGetEN61850EthTypeAppId()
{
    QByteArray ba;
    QString as;
    cZDSP1Client* cl = GetClient(m_actualSocket);
    if (cl->DspVarRead(as = "ETHTYPEAPPID,1", &ba)) {
        ulong *dataCount = (ulong*) ba.data(); // data zeigt auf 1*4 byte
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        ts << dataCount[0];
    }
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    return Answer;
}

QString ZDspServer::mSetEN61850PriorityTagged(QChar *s)
{
    QString ss;
    cZDSP1Client* cl = GetClient(m_actualSocket);
    if (! cl->DspVarWrite(ss = QString("ETHPRIORITYTAGGED,%1;").arg(QString(s))) )
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::ack];
    return Answer;
}

QString ZDspServer::mGetEN61850PriorityTagged()
{
    QByteArray ba;
    QString as;
    cZDSP1Client* cl = GetClient(m_actualSocket);
    if (cl->DspVarRead(as = "ETHPRIORITYTAGGED,1", &ba)) {
        ulong *dataCount = (ulong*) ba.data(); // data zeigt auf 1*4 byte
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        ts << dataCount[0];
    }
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    return Answer;
}


QString ZDspServer::mSetEN61850EthSync(QChar *s)
{
    QString ss;
    cZDSP1Client* cl = GetClient(m_actualSocket);
    if (! cl->DspVarWrite(ss = QString("SYNCASDU,%1;").arg(QString(s))) )
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::ack];
    return Answer;
}

QString ZDspServer::mGetEN61850EthSync()
{
    QByteArray ba;
    QString as;
    cZDSP1Client* cl = GetClient(m_actualSocket);
    if (cl->DspVarRead(as = "SYNCASDU,1", &ba)) {
        ulong *dataCount = (ulong*) ba.data(); // data zeigt auf 1*4 byte
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        ts << dataCount[0];
    }
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    return Answer;
}

QString ZDspServer::mSetEN61850DataCount(QChar *s)
{
    QString ss;
    cZDSP1Client* cl = GetClient(m_actualSocket);
    if (! cl->DspVarWrite(ss = QString("ETHDATACOUNT,%1;").arg(QString(s))) )
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::ack];

    return Answer;
}

QString ZDspServer::mGetEN61850DataCount()
{
    QByteArray ba;
    QString as;
    cZDSP1Client* cl = GetClient(m_actualSocket);
    if (cl->DspVarRead(as = "ETHDATACOUNT,2", &ba)) {
        ulong *dataCount = (ulong*) ba.data(); // data zeigt auf 2*4 byte
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        ts << dataCount[0] << "," << dataCount[1];
    }
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    return Answer;
}

QString ZDspServer::mSetEN61850SyncLostCount(QChar *s)
{
    QString ss;
    cZDSP1Client* cl = GetClient(m_actualSocket);
    if (! cl->DspVarWrite(ss = QString("ETHSYNCLOSTCOUNT,%1;").arg(QString(s))) )
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::ack];

    return Answer;
}

QString ZDspServer::mGetEN61850SyncLostCount()
{
    QByteArray ba;
    QString as;
    cZDSP1Client* cl = GetClient(m_actualSocket);
    if (cl->DspVarRead(as = "ETHSYNCLOSTCOUNT,1", &ba))
    {
        ulong *dataCount = (ulong*) ba.data(); // data zeigt auf 1*4 byte
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        ts << dataCount[0];
    }
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    return Answer;
}

QString ZDspServer::mGetEN61850SourceAdr()
{
    QByteArray ba;
    QString as;
    cZDSP1Client* cl = GetClient(m_actualSocket);
    if (cl->DspVarRead(as = "ETHDESTSOURCEADRESS,3", &ba)) {
        ulong* AdrByte = (ulong*) ba.data(); // data zeigt auf 3*4 byte
        ushort adr[6];  // dest, source address  sind je 6 byte
        int i;
        for (i = 0;i < 2;i++) adr[i] = ( AdrByte[1] >> ((1-i) * 8) ) & 0xFF;  // dsp byte order
        for (i = 0;i < 4;i++) adr[2+i] = ( AdrByte[2] >> ((3-i) * 8) ) & 0xFF; // -> network byte order
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        for (i = 0; i < 5; i++) ts << adr[i] << ",";
        ts << adr[i] << ";";
    }
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
    return Answer;
}

QString ZDspServer::mGetEN61850DestAdr()
{
    cZDSP1Client* cl = GetClient(m_actualSocket);
    QByteArray ba;
    QString as;
    if (cl->DspVarRead(as = "ETHDESTSOURCEADRESS,3", &ba)) {
        ulong* AdrByte = (ulong*) ba.data(); // data zeigt auf 3*4 byte
        ushort adr[6];  // dest, source address  sind je 6 byte
        int i;
        for (i = 0;i < 4;i++) adr[i] = ( AdrByte[0] >> ((3-i) * 8) ) & 0xFF;  // dsp byte order
        for (i = 0;i < 2;i++) adr[4+i] = ( AdrByte[1] >> ((3-i) * 8) ) & 0xFF; // -> network byte order
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        for (i = 0; i < 5; i++) ts << adr[i] << ",";
        ts << adr[i] << ";";
    }
    else
        Answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
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
    QString s;
    if (! cl->DspVar(s = "DSPACK",stat))
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
    return mCommand2Dsp(ss = QString("DSPCMDPAR,4,%1;").arg(par)); // liste mit prozessNr u. HKSK
}

QString ZDspServer::mTriggerIntListALL(QChar *)
{
    QString ss;
    return mCommand2Dsp(ss = QString("DSPCMDPAR,1;"));
}


QString ZDspServer::mResetMaxima(QChar *)
{
    QString ss;
    return mCommand2Dsp(ss =  QString("DSPCMDPAR,3;"));
}

QString ZDspServer::getLcaAndDspVersion()
{
    // LCA
    int rawLcaVersion = m_dspDevNode->ioctlDspIoRead(DspDeviceNode::VersionNr);
    if ( rawLcaVersion < 0 ) {
        qWarning("Error %d reading device version: %s", rawLcaVersion, qPrintable(m_sDspDeviceNode));
        return ZSCPI::scpiAnswer[ZSCPI::errexec]; // fehler bei der ausführung
    }
    // DSP
    cZDSP1Client* client = GetClient(m_actualSocket);
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
    read(pipeFD[0], dummy, 1); // first we read the pipe

    if (!clientlist.isEmpty()) { // wenn vorhanden nutzen wir immer den 1. client zum lesen
        cZDSP1Client *client = clientlist.first();
        QByteArray ba;
        QString s = "CTRLCMDPAR,20";
        if (client->DspVarRead(s, &ba)) { // 20 worte lesen
            ulong* pardsp = (ulong*) ba.data();
            int n = pardsp[0]; // anzahl der interrupts
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

                        client2->m_pNetClient->sendMessage(protobufIntMessage);
                    }
                    else {
                        QByteArray block;
                        QDataStream out(&block, QIODevice::WriteOnly);
                        out.setVersion(QDataStream::Qt_4_0);
                        out << (qint32)0;

                        out << s.toUtf8();
                        out.device()->seek(0);
                        out << (qint32)(block.size() - sizeof(qint32));

                        XiQNetPeer* pNetclient = client2->m_pNetClient;
                        if (pNetclient == nullptr)
                            m_pSCPISocket->write(block);
                        else
                            pNetclient->getTcpSocket()->write(block);
                    }
                }
            }
        }
        client->DspVarWrite(s = QString("CTRLACK,%1;").arg(CmdDone)); // jetzt in jedem fall acknowledge
    }
    else {
        cZDSP1Client dummyClient(0, 0, this); // dummyClient einrichten
        QString s = QString("CTRLACK,%1;").arg(CmdDone);
        dummyClient.DspVarWrite(s); // und rücksetzen
    }
}


bool ZDspServer::BuildDSProgram(QString &errs)
{
    // die programmlisten aller aktiven clients zusammen bauen
    bool ok;
    ulong umo = dm32UserWorkSpace.StartAdr; // usermememory offset

    CmdMem.clear();
    CmdIntMem.clear();
    QDataStream mds1 ( &CmdMem, QIODevice::Unbuffered | QIODevice::ReadWrite );
    mds1.setByteOrder(QDataStream::LittleEndian);
    QDataStream mds2 ( &CmdIntMem, QIODevice::Unbuffered | QIODevice::ReadWrite );
    mds2.setByteOrder(QDataStream::LittleEndian);

    cDspCmd cmd;
    QString s;

    if (clientlist.count() > 0) {
        cZDSP1Client* client = clientlist.at(0);
        s =  QString( "DSPMEMOFFSET(%1)" ).arg(dm32DspWorkspace.StartAdr);
        cmd = client->GenDspCmd(s, &ok, 0, 0);
        mds1 << cmd;
        for (int i = 0; i < clientlist.count(); i++) {
            client = clientlist.at(i);
            if (client->isActive()) {
                s =  QString( "USERMEMOFFSET(%1)" ).arg(umo);
                cmd = client->GenDspCmd(s, &ok, 0, 0);
                mds1 << cmd;
                mds2 << cmd;

                if (!client->GenCmdLists(errs, umo, UserWorkSpaceGlobalSegmentAdr))
                    return false;

                umo += client->setStartAdr(umo, UserWorkSpaceGlobalSegmentAdr); // relokalisieren der daten im dsp

                QList<cDspCmd> cmdl = client->GetDspCmdList();
                for (int j = 0; j < cmdl.size(); j++ ) mds1 << cmdl[j]; // cycl. liste
                QList<cDspCmd> cmdl2 = client->GetDspIntCmdList();
                for ( int j = 0; j < cmdl2.size(); j++ ) mds2 << cmdl2[j]; // interrupt liste
            }
        }

        client = clientlist.at(0);
        s = QString( "DSPINTPOST()"); // wir triggern das senden der serialisierten interrupts
        cmd = client->GenDspCmd(s, &ok, 0, 0);
        mds1 << cmd;
    }

    cZDSP1Client dummyClient(0, 0, this); // dummyClient einrichten damit was jetzt kommt noch
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

    cZDSP1Client dummyClient(0, 0, this); // dummyClient einrichten zum laden der kette

    ulong offset = dummyClient.m_dspVarResolver.adr(s) ;
    if (DspDevSeek(offset) < 0 )  // startadr im treiber setzen
        return false;

    if (DspDevWrite(CmdMem.data(), CmdMem.size()) < 0)
        return false;

    offset = dummyClient.m_dspVarResolver.adr(s2) ;
    if (DspDevSeek(offset) < 0 )  // startsadr im treiber setzen
        return false;

    if (DspDevWrite(CmdIntMem.data(), CmdIntMem.size()) < 0)
        return false;

    QString ss;
    mCommand2Dsp(ss = QString("DSPCMDPAR,7,%1;").arg(ActivatedCmdList));
    // dem dsp die neue liste mitteilen
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
        Answer = QString("%1 %2").arg(ZSCPI::scpiAnswer[ZSCPI::errval]).arg(errs); // das "fehlerhafte" kommando anhängen
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

static constexpr int DSP_RUNNING = 0x80;

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
    if ( r == DspDeviceNode::MAGIC_ID21262 ) {
        UserWorkSpaceGlobalSegmentAdr = dm32UserWorkSpaceGlobal21262;
        return m_sDspBootPath.contains("zdsp21262.ldr");
        // adressen im dsp stehen für adsp21262 default richtig
    }
    else if ( r == DspDeviceNode::MAGIC_ID21362) {
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
    return m_dspDevNode->ioctlDspIoRead(DspDeviceNode::MagicId);
}

bool ZDspServer::Test4HWPresent()
{
    int r = readMagicId();
    return ( (r == DspDeviceNode::MAGIC_ID21262) || (r == DspDeviceNode::MAGIC_ID21362));
}

bool ZDspServer::Test4DspRunning()
{
    int r = m_dspDevNode->ioctlDspIoRead(DspDeviceNode::DSPStat);
    return ((r & DSP_RUNNING) > 0);
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
    if (clientlist.count() > 0) {
        for (int i = 0; i < clientlist.count(); i++) {
            cZDSP1Client* client = clientlist.at(i);
            if (client->getSocket() == s)
                return client;
        }
    }
    return nullptr;
}

cZDSP1Client* ZDspServer::GetClient(XiQNetPeer *peer)
{
    if (clientlist.count() > 0) {
        for (int i = 0; i < clientlist.count(); i++) {
            cZDSP1Client* client = clientlist.at(i);
            if (client->m_pNetClient == peer)
                return client;
        }
    }
    return nullptr;
}

void ZDspServer::onEstablishNewConnection(XiQNetPeer *newClient)
{
    connect(newClient, &XiQNetPeer::sigMessageReceived, this, &ZDspServer::onExecuteCommandProto);
    connect(newClient, &XiQNetPeer::sigConnectionClosed, this, &ZDspServer::deleteConnection);
    AddClient(newClient); // we additionally add the client to our list
}

void ZDspServer::deleteConnection()
{
    XiQNetPeer* client = qobject_cast<XiQNetPeer*>(sender());
    DelClient(client);
}

void ZDspServer::onExecuteCommandProto(std::shared_ptr<google::protobuf::Message> cmd)
{
    XiQNetPeer* client = qobject_cast<XiQNetPeer*>(sender());
    std::shared_ptr<ProtobufMessage::NetMessage> protobufCommand = std::static_pointer_cast<ProtobufMessage::NetMessage>(cmd);
    if ( (protobufCommand != 0) && (client != 0)) {
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
                cZDSP1Client *zdspclient = AddClient(client); // we add a new client with the same socket but different identifier
                m_zdspdClientHash[clientId] = zdspclient;
                m_clientIDHash[zdspclient] = clientId; // we need this list in case of interrupts
            }

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
            client->sendMessage(protobufAnswer);
        }
        else {
            m_actualSocket = GetClient(client)->getSocket();

            QString input =  QString::fromStdString(protobufCommand->scpi().command());
            QString output = m_cmdInterpreter->CmdExecute(input);

            QByteArray block;
            QDataStream out(&block, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_0);
            out << (qint32)0;

            out << output.toUtf8();
            out.device()->seek(0);
            out << (qint32)(block.size() - sizeof(qint32));

            client->getTcpSocket()->write(block);
        }
    }
}

void ZDspServer::setSCPIConnection()
{
    m_pSCPISocket = m_pSCPIServer->nextPendingConnection();
    m_pSCPIClient = AddSCPIClient();
    connect(m_pSCPISocket, &QIODevice::readyRead, this, &ZDspServer::SCPIInput);
    connect(m_pSCPISocket, &QAbstractSocket::disconnected, this, &ZDspServer::SCPIdisconnect);
}

void ZDspServer::SCPIInput()
{
    QString m_sInput;
    while ( m_pSCPISocket->canReadLine() )
        m_sInput += m_pSCPISocket->readLine();
    m_sInput.remove('\r'); // we remove cr lf
    m_sInput.remove('\n');
    QString m_sOutput = m_cmdInterpreter->CmdExecute(m_sInput) + "\n";
    QByteArray ba = m_sOutput.toLatin1();
    m_pSCPISocket->write(ba);
}

void ZDspServer::SCPIdisconnect()
{
    disconnect(m_pSCPISocket, 0, 0, 0); // we disconnect everything
    DelSCPIClient();
}

cZDSP1Client* ZDspServer::AddClient(XiQNetPeer* m_pNetClient)
{
    // fügt einen client hinzu
    m_nSocketIdentifier++;
    if (m_nSocketIdentifier == 0)
        m_nSocketIdentifier++;
    cZDSP1Client* client = new cZDSP1Client(m_nSocketIdentifier, m_pNetClient,this);
    clientlist.append(client);
    return client;
}

void ZDspServer::DelClient(XiQNetPeer* netClient)
{ // entfernt alle cZDSP1Clients die an diesem netClient kleben
    QList<cZDSP1Client*> todeleteList;
    for (int i = 0; i < clientlist.count(); i++) {
        cZDSP1Client* zdspclient = clientlist.at(i);
        XiQNetPeer* peer = zdspclient->m_pNetClient;
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
        clientlist.removeOne(client);
        delete client;
    }
    LoadDSProgram(); // after deleting clients we reload dsp program
}

void ZDspServer::DelClient(QByteArray clientId)
{
    if (m_zdspdClientHash.contains(clientId)) {
        cZDSP1Client *client = m_zdspdClientHash.take(clientId);
        m_clientIDHash.remove(client);
        clientlist.removeOne(client);
        delete client;
        LoadDSProgram(); // after deleting client we reload dsp program ... means unload dsp for this client
    }
}

cZDSP1Client *ZDspServer::AddSCPIClient()
{
    return AddClient(0); // we add this client with netclient (XiQNetPeer) = 0 because it is no XiQNetPeer but
}

void ZDspServer::DelSCPIClient()
{
    clientlist.removeAll(m_pSCPIClient);
    LoadDSProgram(); // after deleting client we reload dsp program ... means unload dsp for this client
}

QString ZDspServer::SCPICmd(SCPICmdType cmd, QChar *s)
{
    switch ((int)cmd)
    {
    case    TestDsp:            return mTestDsp(s);
    case 	ResetDsp:           return mResetDsp(s);
    case	BootDsp:            return mBootDsp(s);
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
    case   SetEN61850DestAdr:    	return mSetEN61850DestAdr(s);
    case   SetEN61850SourceAdr:    	return mSetEN61850SourceAdr(s);
    case 	SetEN61850EthTypeAppId:  return mSetEN61850EthTypeAppId(s);
    case 	SetEN61850PriorityTagged:  return mSetEN61850PriorityTagged(s);
    case   SetEN61850EthSync: 	return mSetEN61850EthSync(s);
    case   SetDspCommandStat:	return mSetDspCommandStat(s);
    case   SetEN61850DataCount:	return mSetEN61850DataCount(s);
    case   SetEN61850SyncLostCount:	return mSetEN61850SyncLostCount(s);
    case   TriggerIntListHKSK:	return mTriggerIntListHKSK(s);
    case   TriggerIntListALL:		return mTriggerIntListALL(s);
    case   ResetMaxima:		return mResetMaxima(s);
    case   ResetDeviceLoadMax:	return mResetDeviceLoadMax();
    }
    Answer = "ProgrammierFehler"; // hier sollten wir nie hinkommen

    return Answer;
}

QString ZDspServer::SCPIQuery( SCPICmdType cmd)
{
    switch ((int)cmd)
    {
    case 		GetPCBSerialNumber: 	return mGetPCBSerialNumber();
    case 		GetDeviceVersion:		return getLcaAndDspVersion();
    case 		GetServerVersion: 		return getServerVersion();
    case		GetDeviceLoadMax: 	return mGetDeviceLoadMax();
    case 		GetDeviceLoadAct: 	return mGetDeviceLoadAct();
    case		GetDspStatus:		return mGetDspStatus();
    case 		GetDeviceStatus: 		return mGetDeviceStatus();
    case 		GetRavList: 		return getRawActualValueList();
    case 		GetCmdIntList: 		return mGetCmdIntList();
    case 		GetCmdList: 		return mGetCmdList();
    case		GetSamplingSystem:	return mGetSamplingSystem();
    case        GetCommEncryption:	return mGetCommEncryption();
    case		GetEN61850DestAdr:	return mGetEN61850DestAdr();
    case		GetEN61850SourceAdr:	return mGetEN61850SourceAdr();
    case		GetEN61850EthTypeAppId:  return mGetEN61850EthTypeAppId();
    case		GetEN61850PriorityTagged:  return mGetEN61850PriorityTagged();
    case                 GetEN61850EthSync: 	return mGetEN61850EthSync();
    case		GetEN61850DataCount:	return mGetEN61850DataCount();
    case		GetEN61850SyncLostCount:	return mGetEN61850SyncLostCount();
    case 		GetDspCommandStat:	return mGetDspCommandStat();
    }
    Answer = "ProgrammierFehler"; // hier sollten wir nie hinkommen
    return Answer;
}