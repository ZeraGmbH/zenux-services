// implementation des eigenlichen servers zdsp1(d)

#include "zdspclient.h"
#include "zeraglobal.h"
#include "zdspglobal.h"
#include "zhserver.h"
#include "zdsp1d.h"
#include "dsp.h"
#include "parse-zdsp.h"
#include <QDebug>
#include <QCoreApplication>
#include <QFinalState>
#include <QDataStream>
#include <QFile>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTextStream>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef SYSTEMD_NOTIFICATION
#include <systemd/sd-daemon.h>
#endif

#define ADSP_IOC_MAGIC 'a'
/* ioctl commands */
#define ADSP_RESET _IOR(ADSP_IOC_MAGIC,1,char*)
#define ADSP_BOOT _IOR(ADSP_IOC_MAGIC,2,char*)
#define ADSP_INT_REQ _IOR(ADSP_IOC_MAGIC,3,char*)
#define ADSP_INT_ENABLE _IOR(ADSP_IOC_MAGIC,4,char*)
#define ADSP_INT_DISABLE _IOR(ADSP_IOC_MAGIC,5,char*)
#define IO_READ _IOR(ADSP_IOC_MAGIC,6,char*)

extern sMemSection dm32DspWorkspace;
extern sMemSection dm32DialogWorkSpace;
extern sMemSection dm32UserWorkSpace;
extern sMemSection dm32CmdList;
extern sMemSection symbConsts1;

extern sDspVar CmdListVar;
extern sDspVar UserWorkSpaceVar;

extern cNode* InitCmdTree();


static char devavail[6] = "avail";
static char devnavail[10]= "not avail";

static char dsprunning[8] = "running";
static char dspnrunning[12]= "not running";



/* globaler zeiger auf  "den"  server und eine signal behandlungsroutine */
cZDSP1Server* DSPServer;
int pipeFD[2];
char pipeBuf[2] = "I";
//int anzInt = 0;

void SigHandler(int)
{
    write(pipeFD[1], pipeBuf, 1);
}


struct sigaction mySigAction;
// sigset_t mySigmask, origSigmask;


cZDSP1Server::cZDSP1Server()
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

    QObject::connect(statexmlConfiguration, &QAbstractState::entered, this, &cZDSP1Server::doConfiguration);
    QObject::connect(statesetupServer, &QAbstractState::entered, this, &cZDSP1Server::doSetupServer);
    QObject::connect(m_stateconnect2RM, &QAbstractState::entered, this, &cZDSP1Server::doConnect2RM);
    QObject::connect(m_stateconnect2RMError, &QAbstractState::entered, this, &cZDSP1Server::connect2RMError);
    QObject::connect(m_stateSendRMIdentAndRegister, &QAbstractState::entered, this, &cZDSP1Server::doIdentAndRegister);
    QObject::connect(stateFINISH, &QAbstractState::entered, this, &cZDSP1Server::doCloseServer);

    m_pInitializationMachine->start();
}

cZDSP1Server::~cZDSP1Server()
{
    delete m_pDebugSettings;
    delete  m_pETHSettings;
    delete m_pDspSettings;
    if (clientlist.count() > 0)
        for (int i = 0; i < clientlist.count(); i++) {
            cZDSP1Client* cl = clientlist.at(i);
            delete cl;
        }
    resetDsp(); // we reset the dsp when we close the server
    close(m_devFileDescriptor); // close dev.
    close(pipeFD[0]);
    close(pipeFD[1]);
}

void cZDSP1Server::doConfiguration()
{
    QStringList args = QCoreApplication::instance()->arguments();
    if (args.count() != 2) { // we want exactly 1 parameter
        m_nerror = parameterError;
        emit abortInit();
    }
    else {
        if ( pipe(pipeFD) == -1 ) {
            m_nerror = pipeError;
            emit abortInit();
        }
        else {
            fcntl( pipeFD[1], F_SETFL, O_NONBLOCK);
            fcntl( pipeFD[0], F_SETFL, O_NONBLOCK);
            m_pNotifier = new QSocketNotifier(pipeFD[0], QSocketNotifier::Read, this);
            connect(m_pNotifier, &QSocketNotifier::activated, this, &cZDSP1Server::DspIntHandler);
            if (myXMLConfigReader->loadSchema(defaultXSDFile)) {
                // we want to initialize all settings first
                m_pDebugSettings = new cDebugSettings(myXMLConfigReader);
                connect(myXMLConfigReader,&Zera::XMLConfig::cReader::valueChanged,m_pDebugSettings,&cDebugSettings::configXMLInfo);
                m_pETHSettings = new EthSettings(myXMLConfigReader);
                connect(myXMLConfigReader,&Zera::XMLConfig::cReader::valueChanged,m_pETHSettings,&EthSettings::configXMLInfo);
                m_pDspSettings = new cDSPSettings(myXMLConfigReader);
                connect(myXMLConfigReader,&Zera::XMLConfig::cReader::valueChanged,m_pDspSettings,&cDSPSettings::configXMLInfo);

                QString s = args.at(1);
                if(!myXMLConfigReader->loadXMLFile(s)) {
                    m_nerror = xmlfileError;
                    emit abortInit();
                }
            }
            else {
                m_nerror = xsdfileError;
                emit abortInit();
            }
        }
    }
}


void cZDSP1Server::doSetupServer()
{
    cParseZdsp* parser = new(cParseZdsp); // das ist der parser
    pCmdInterpreter=new cCmdInterpreter(this,InitCmdTree(), parser); // das ist der kommando interpreter
    m_sDspDeviceVersion = m_sDspSerialNumber = "Unknown"; // kennen wir erst mal nicht
    m_sDspBootPath = m_pDspSettings->getBootFile();
    DSPServer = this;
    ActivatedCmdList = 0; // der derzeit aktuelle kommando listen satz (0,1)

    myProtonetServer =  new XiQNetServer(this);
    myProtonetServer->setDefaultWrapper(&m_ProtobufWrapper);
    connect(myProtonetServer, &XiQNetServer::sigClientConnected, this, &cZDSP1Server::onEstablishNewConnection);
    myProtonetServer->startServer(m_pETHSettings->getPort(EthSettings::protobufserver)); // and can start the server now

    if (m_pETHSettings->isSCPIactive()) {
        m_pSCPIServer = new QTcpServer(this);
        m_pSCPIServer->setMaxPendingConnections(1); // we only accept 1 client to connect
        connect(m_pSCPIServer, &QTcpServer::newConnection, this, &cZDSP1Server::setSCPIConnection);
        m_pSCPIServer->listen(QHostAddress::AnyIPv4, m_pETHSettings->getPort(EthSettings::scpiserver));
    }
    m_sDspDeviceNode = m_pDspSettings->getDeviceNode(); // we try to open the dsp device
    if (DspDevOpen() < 0) {
        m_nerror = dspDeviceError; // and finish if not possible
        emit abortInit();
    }
    else {
        mySigAction.sa_handler = &SigHandler; // signal handler einrichten
        sigemptyset(&mySigAction.sa_mask);
        mySigAction. sa_flags = SA_RESTART;
        mySigAction.sa_restorer = NULL;
        sigaction(SIGIO, &mySigAction, NULL); // handler für sigio definieren
        SetFASync();
        m_retryRMConnect = 100;
        m_retryTimer.setSingleShot(true);
        connect(&m_retryTimer, &QTimer::timeout, this, &cZDSP1Server::sigServerIsSetUp);

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
                        m_nerror = dspSetSamplingError;
                        emit abortInit();
                    }
                }
                else {
                    m_nerror = dspBootError;
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
            m_nerror = dspBootFileError;
            emit abortInit();
        }
    }
}

void cZDSP1Server::doCloseServer()
{
    QCoreApplication::instance()->exit(m_nerror);
}

void cZDSP1Server::doConnect2RM()
{
    m_nerror = rmConnectionError; // preset error condition
    m_pRMConnection->connect2RM();
}

void cZDSP1Server::connect2RMError()
{
    m_retryRMConnect--;
    if (m_retryRMConnect == 0)
        emit abortInit();
    else
        m_retryTimer.start(200);
}

void cZDSP1Server::doIdentAndRegister()
{
    m_pRMConnection->SendIdent(sServerName);

    quint32 port = m_pETHSettings->getPort(EthSettings::protobufserver);

    QString cmd, par;

    m_pRMConnection->SendCommand(cmd = QString("RESOURCE:ADD"), par = QString("DSP;DSP1;;ADSP Signal Processor;%1;")
                                 .arg(port));

    sDspVar* pDspVar = &CmdListVar;
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

int cZDSP1Server::DspDevOpen()
{
    if ( (m_devFileDescriptor = open(m_sDspDeviceNode.toLatin1().data(), O_RDWR)) < 0 )
        qWarning("Error opening dsp device: %s", qPrintable(m_sDspDeviceNode));
    return m_devFileDescriptor;
}

int cZDSP1Server::DspDevSeek(ulong adr)
{
    int r = lseek(m_devFileDescriptor, adr, 0);
    if (r < 0)
        qWarning("Error positioning dsp device: %s", qPrintable(m_sDspDeviceNode));
    return r;
}

int cZDSP1Server::DspDevWrite(char* buf, int len)
{
    int r = write(m_devFileDescriptor, buf, len);
    if (r <0 )
        qWarning("Error writing dsp device: %s", qPrintable(m_sDspDeviceNode));
    return r;
}

int cZDSP1Server::DspDevRead(char* buf, int len)
{
    int r = read(m_devFileDescriptor, buf, len);
    if (r < 0 )
        qWarning("Error reading dsp device: %s", qPrintable(m_sDspDeviceNode));
    return r;
}

QString cZDSP1Server::mTestDsp(QChar* s)
{
    QString par = pCmdInterpreter->m_pParser->GetKeyword(&s); // holt den parameter aus dem kommando
    bool ok;
    int tmode = par.toInt(&ok);
    int nr = 0;
    bool tstart = false;
    if ((ok) && ( (tmode>=0) && (tmode<2) )) {
        par = pCmdInterpreter->m_pParser->GetKeyword(&s);
        nr=par.toInt(&ok);
        if ((ok) && ( (nr>=0) && (nr<1000) ))
            tstart = true;
    }
    if (tstart == true) {
        int i,j;
        int errcount = 0;
        switch (tmode)
        {
            case 0:
                for (i=0; i<nr; i++) {
                    mResetDsp(s);
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
                bool err;
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
                cZDSP1Client* cl = GetClient(ActSock);
                QString sadr  = "UWSPACE";
                ulong adr = cl->DspVarResolver.adr(sadr) ;
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
                    err = false;
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
        Answer = ERRVALString; // fehler wert
    return Answer;
}

bool cZDSP1Server::resetDsp()
{
    int r = ioctl(m_devFileDescriptor,ADSP_RESET); // und reset
    if ( r < 0 ) {
        qWarning("error %d reset dsp device: %s", r, qPrintable(m_sDspDeviceNode));
        Answer = ERREXECString; // fehler bei der ausführung
        return false;
    }
    Answer = ACKString;
    return true;
}

QString cZDSP1Server::mResetDsp(QChar*)
{
    resetDsp();
    return Answer;
}

bool cZDSP1Server::bootDsp()
{
    QFile f (m_sDspBootPath);
    if (!f.open(QIODevice::Unbuffered | QIODevice::ReadOnly)) {
        qWarning("error opening dsp boot file: %s", qPrintable(m_sDspBootPath));
        Answer = ERRPATHString;
        return false;
    }
    QByteArray BootMem = f.readAll();
    f.close();
    int r = ioctl(m_devFileDescriptor,ADSP_BOOT,BootMem.data()); // und booten
    if ( r < 0 ) {
        qWarning("error %d booting dsp device: %s", r, qPrintable(m_sDspDeviceNode));
        Answer = ERREXECString; // fehler bei der ausführung
        return false;
    }
    Answer = ACKString;
    return true;
}

bool cZDSP1Server::setSamplingSystem()
{
    QString ss;
    for (int i = 0; i < 10; i++) // we try max. 10 times to set .... this should work
    {
        mCommand2Dsp(ss = QString("DSPCMDPAR,2,%1,%2,%3;").arg(m_pDspSettings->getChannelNr())
                                                                     .arg(m_pDspSettings->getSamplesSignalPeriod())
                                                                     .arg(m_pDspSettings->getsamplesMeasurePeriod()));
        if (Answer == ACKString)
            return true;
        usleep(10000); // give dsp a bit time before next try
    }
    return false;
}

QString cZDSP1Server::mBootDsp(QChar *)
{
    bootDsp();
    return Answer;
}

QString cZDSP1Server::mGetPCBSerialNumber()
{
    return m_sDspSerialNumber;
}

QString cZDSP1Server::mCommand2Dsp(QString& qs)
{
    // we need a client to do the job
    cZDSP1Client cl(0, 0, this);
    do
    {
        Answer = ERREXECString;
        int ack;

        QString ss;
        if (! cl.DspVar( ss ="DSPACK",ack)) break;

        if ( ack ==  InProgress)
        {
            Answer = BUSYString;
            break;
        }

        if (! cl.DspVarWrite(ss = "DSPACK,0;") ) break; // reset acknowledge
        if (! cl.DspVarWrite(qs)) break; // kommando und parameter -> dsp

        ioctl(m_devFileDescriptor,ADSP_INT_REQ); // interrupt beim dsp auslösen
        Answer = ACKString; // sofort fertig melden ....sync. muss die applikation

    } while (0);
    return Answer;
}

QString cZDSP1Server::mSetSamplingSystem(QChar *s)
{
    QString ss;

    return mCommand2Dsp(ss = QString("DSPCMDPAR,2,%1;").arg(QString(s)));
}

QString cZDSP1Server::mSetCommEncryption(QChar *s)
{
    bool ok;
    QString par = pCmdInterpreter->m_pParser->GetKeyword(&s); // holt den parameter aus dem kommando
    int enc=par.toInt(&ok);
    if ((ok) && ( (enc==0) || (enc==1) ))
    {
        cZDSP1Client* cl = GetClient(ActSock);
        cl->SetEncryption(enc);
        Answer = ACKString; // acknowledge
    }
    else
        Answer = ERRVALString; // fehler wert

    return Answer;
}

QString cZDSP1Server::mGetSamplingSystem()
{
    do
    {
        Answer = ERREXECString;
        int n, ss, sm;

        cZDSP1Client* cl = GetClient(ActSock);

        QString s;
        if (! cl->DspVar(s = "NCHANNELS",n)) break;
        if (! cl->DspVar(s = "NSPERIOD",ss)) break;
        if (! cl->DspVar(s = "NSMEAS",sm)) break;

        Answer = QString("%1,%2,%3").arg(n).arg(ss).arg(sm);
    } while (0);

    return Answer;
}

QString cZDSP1Server::mGetCommEncryption()
{
    cZDSP1Client* cl = GetClient(ActSock);
    Answer = QString::number(cl->GetEncryption());

    return Answer;
}

QString cZDSP1Server::mSetEN61850SourceAdr(QChar* s)
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
    if (!ok) Answer = NACKString;
    else
        do
    {
        Answer = ERREXECString; // vorbesetzen
        QString as;
        cZDSP1Client* cl = GetClient(ActSock);
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

QString cZDSP1Server::mSetEN61850DestAdr(QChar *s)
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
        Answer = NACKString;
    else
        do {
            Answer = ERREXECString; // vorbesetzen
            cZDSP1Client* cl = GetClient(ActSock);
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

QString cZDSP1Server::mSetEN61850EthTypeAppId(QChar *s)
{
    QString ss;
    cZDSP1Client* cl = GetClient(ActSock);
    if (! cl->DspVarWrite(ss = QString("ETHTYPEAPPID,%1;").arg(QString(s))) )
        Answer = ERREXECString;
    else
        Answer = ACKString;
    return Answer;
}

QString cZDSP1Server::mGetEN61850EthTypeAppId()
{
    QByteArray ba;
    QString as;
    cZDSP1Client* cl = GetClient(ActSock);
    if (cl->DspVarRead(as = "ETHTYPEAPPID,1", &ba)) {
        ulong *dataCount = (ulong*) ba.data(); // data zeigt auf 1*4 byte
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        ts << dataCount[0];
    }
    else
        Answer = ERREXECString;
    return Answer;
}

QString cZDSP1Server::mSetEN61850PriorityTagged(QChar *s)
{
    QString ss;
    cZDSP1Client* cl = GetClient(ActSock);
    if (! cl->DspVarWrite(ss = QString("ETHPRIORITYTAGGED,%1;").arg(QString(s))) )
        Answer = ERREXECString;
    else
        Answer = ACKString;
    return Answer;
}

QString cZDSP1Server::mGetEN61850PriorityTagged()
{
    QByteArray ba;
    QString as;
    cZDSP1Client* cl = GetClient(ActSock);
    if (cl->DspVarRead(as = "ETHPRIORITYTAGGED,1", &ba)) {
        ulong *dataCount = (ulong*) ba.data(); // data zeigt auf 1*4 byte
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        ts << dataCount[0];
    }
    else
        Answer = ERREXECString;
    return Answer;
}


QString cZDSP1Server::mSetEN61850EthSync(QChar *s)
{
    QString ss;
    cZDSP1Client* cl = GetClient(ActSock);
    if (! cl->DspVarWrite(ss = QString("SYNCASDU,%1;").arg(QString(s))) )
        Answer = ERREXECString;
    else
        Answer = ACKString;
    return Answer;
}

QString cZDSP1Server::mGetEN61850EthSync()
{
    QByteArray ba;
    QString as;
    cZDSP1Client* cl = GetClient(ActSock);
    if (cl->DspVarRead(as = "SYNCASDU,1", &ba)) {
        ulong *dataCount = (ulong*) ba.data(); // data zeigt auf 1*4 byte
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        ts << dataCount[0];
    }
    else
        Answer = ERREXECString;
    return Answer;
}

QString cZDSP1Server::mSetEN61850DataCount(QChar *s)
{
    QString ss;
    cZDSP1Client* cl = GetClient(ActSock);
    if (! cl->DspVarWrite(ss = QString("ETHDATACOUNT,%1;").arg(QString(s))) )
        Answer = ERREXECString;
    else
        Answer = ACKString;

    return Answer;
}

QString cZDSP1Server::mGetEN61850DataCount()
{
    QByteArray ba;
    QString as;
    cZDSP1Client* cl = GetClient(ActSock);
    if (cl->DspVarRead(as = "ETHDATACOUNT,2", &ba)) {
        ulong *dataCount = (ulong*) ba.data(); // data zeigt auf 2*4 byte
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        ts << dataCount[0] << "," << dataCount[1];
    }
    else
        Answer = ERREXECString;
    return Answer;
}

QString cZDSP1Server::mSetEN61850SyncLostCount(QChar *s)
{
    QString ss;
    cZDSP1Client* cl = GetClient(ActSock);
    if (! cl->DspVarWrite(ss = QString("ETHSYNCLOSTCOUNT,%1;").arg(QString(s))) )
        Answer = ERREXECString;
    else
        Answer = ACKString;

    return Answer;
}

QString cZDSP1Server::mGetEN61850SyncLostCount()
{
    QByteArray ba;
    QString as;
    cZDSP1Client* cl = GetClient(ActSock);
    if (cl->DspVarRead(as = "ETHSYNCLOSTCOUNT,1", &ba))
    {
        ulong *dataCount = (ulong*) ba.data(); // data zeigt auf 1*4 byte
        Answer = "";
        QTextStream ts( &Answer, QIODevice::WriteOnly );
        ts << dataCount[0];
    }
    else
        Answer = ERREXECString;
    return Answer;
}

QString cZDSP1Server::mGetEN61850SourceAdr()
{
    QByteArray ba;
    QString as;
    cZDSP1Client* cl = GetClient(ActSock);
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
        Answer = ERREXECString;
    return Answer;
}

QString cZDSP1Server::mGetEN61850DestAdr()
{
    cZDSP1Client* cl = GetClient(ActSock);
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
        Answer = ERREXECString;
    return Answer;
}

QString cZDSP1Server::mSetDspCommandStat(QChar *s)
{
    Answer = ERREXECString;
    QString ss;
    cZDSP1Client* cl = GetClient(ActSock);
    if (! cl->DspVarWrite(ss = QString("DSPACK,%1;").arg(QString(s))) )
        Answer = ERREXECString;
    else
        Answer = ACKString;
    return Answer;
}

QString cZDSP1Server::mGetDspCommandStat()
{
    int stat;
    cZDSP1Client* cl = GetClient(ActSock);
    QString s;
    if (! cl->DspVar(s = "DSPACK",stat))
        Answer = ERREXECString;
    else
        Answer = QString("%1").arg(stat);
    return Answer;
}

QString cZDSP1Server::mTriggerIntListHKSK(QChar *s)
{
    QString ss(s);
    ulong par = ss.toULong();
    par = (par & 0xFFFF )| (ActSock << 16);
    return mCommand2Dsp(ss = QString("DSPCMDPAR,4,%1;").arg(par)); // liste mit prozessNr u. HKSK
}

QString cZDSP1Server::mTriggerIntListALL(QChar *)
{
    QString ss;
    return mCommand2Dsp(ss = QString("DSPCMDPAR,1;"));
}


QString cZDSP1Server::mResetMaxima(QChar *)
{
    QString ss;
    return mCommand2Dsp(ss =  QString("DSPCMDPAR,3;"));
}

QString cZDSP1Server::mGetDeviceVersion()
{
    int r = ioctl(m_devFileDescriptor,IO_READ,VersionNr);
    if ( r < 0 ) {
        qWarning("Error %d reading device version: %s", r, qPrintable(m_sDspDeviceNode));
        Answer = ERREXECString; // fehler bei der ausführung
        return Answer.toLatin1().data();
    }
    cZDSP1Client* cl = GetClient(ActSock);
    QString p = "VNR,1;";
    p = cl->DspVarListRead(p);  // ab "VNR"  1 wort lesen
    p = p.section(':',1,1);
    p = p.remove(';');
    double d = p.toDouble();
    m_sDspDeviceVersion = QString("DSPLCA: V%1.%2;DSP V%3").arg((r >>8) & 0xff).arg(r & 0xff).arg(d,0,'f',2);
    return m_sDspDeviceVersion;
}

QString cZDSP1Server::mGetServerVersion()
{
    return sSoftwareVersion;
}

QString cZDSP1Server::mGetDspStatus()
{
    if ( Test4DspRunning() )
        Answer = dsprunning;
    else
        Answer = dspnrunning;
    return Answer;
}

QString cZDSP1Server::mGetDeviceStatus()
{
    if ( Test4HWPresent() )
        Answer = devavail;
    else
        Answer = devnavail;
    return Answer;
}

QString cZDSP1Server::mGetDeviceLoadAct()
{
    cZDSP1Client* cl = GetClient(ActSock);
    QString p = "BUSY,1;";
    Answer = cl->DspVarListRead(p);  // ab "BUSY"  1 wort lesen
    return Answer;
}

QString cZDSP1Server::mGetDeviceLoadMax()
{
    cZDSP1Client* cl = GetClient(ActSock);
    QString p = "BUSYMAX,1;";
    Answer = cl->DspVarListRead(p);  // ab "BUSYMAX"  1 wort lesen
    return Answer;
}

QString cZDSP1Server::mResetDeviceLoadMax()
{
    cZDSP1Client* cl = GetClient(ActSock);
    QString p = "BUSYMAX,0.0";
    Answer = cl->DspVarWriteRM(p);
    return Answer;
}

QDataStream& operator<<(QDataStream& ds,cDspCmd c)
{
    ds << (quint32) c.w[0] << (quint32) c.w[1];
    return ds;
}

void cZDSP1Server::DspIntHandler(int)
{
    char buf[2];
    read(pipeFD[0], buf, 1); // first we read the pipe
    cZDSP1Client *client,*client2;
    if ((!clientlist.isEmpty()) && (client = clientlist.first()) !=0) { // wenn vorhanden nutzen wir immer den 1. client zum lesen
        QByteArray ba;
        QString s = "CTRLCMDPAR,20";
        if (client->DspVarRead(s, &ba)) { // 20 worte lesen
            ulong* pardsp = (ulong*) ba.data();
            int n = pardsp[0]; // anzahl der interrupts
            for (int i = 1; i < (n+1); i++) {
                int process = pardsp[i] >> 16;
                if ((client2 = GetClient(process)) !=0) { // gibts den client noch, der den interrupt haben wollte
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

                        XiQNetPeer* pNetclient;
                        pNetclient = client2->m_pNetClient;

                        if (pNetclient == 0)
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
        cZDSP1Client *dummyClient = new cZDSP1Client(0, 0, this); // dummyClient einrichten
        QString s = QString("CTRLACK,%1;").arg(CmdDone);
        dummyClient->DspVarWrite(s); // und rücksetzen
        delete dummyClient;
    }
}


bool cZDSP1Server::BuildDSProgram(QString &errs)
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
    cZDSP1Client* client;
    cDspCmd cmd;
    QString s;

    if (clientlist.count() > 0) {
        s =  QString( "DSPMEMOFFSET(%1)" ).arg(dm32DspWorkspace.StartAdr);
        client = clientlist.at(0);
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

    client = new cZDSP1Client(0, 0, this); // dummyClient einrichten damit was jetzt kommt noch

    s =  QString( "INVALID()"); // funktioniert selbst wenn wenn wir keinen mehr haben
    cmd = client->GenDspCmd(s, &ok, 0, 0);
    mds1 << cmd; // kommando listen ende
    mds2 << cmd;

    delete client;

    return true;
}


bool cZDSP1Server::LoadDSProgram()
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

    cZDSP1Client* client = new cZDSP1Client(0, 0, this); // dummyClient einrichten zum laden der kette

    ulong offset = client->DspVarResolver.adr(s) ;
    if (DspDevSeek(offset) < 0 )  // startadr im treiber setzen
        return false;

    if (DspDevWrite(CmdMem.data(), CmdMem.size()) < 0)
        return false;

    offset = client->DspVarResolver.adr(s2) ;
    if (DspDevSeek(offset) < 0 )  // startsadr im treiber setzen
        return false;

    if (DspDevWrite(CmdIntMem.data(), CmdIntMem.size()) < 0)
        return false;

    QString ss;
    mCommand2Dsp(ss = QString("DSPCMDPAR,7,%1;").arg(ActivatedCmdList));
    // dem dsp die neue liste mitteilen
    delete client;
    return true;
}

QString cZDSP1Server::mUnloadCmdList(QChar *)
{
    QString error;
    cZDSP1Client* cl = GetClient(ActSock);
    cl->SetActive(false);
    BuildDSProgram(error); // wir bauen neu
    if (!LoadDSProgram()) // und laden
        Answer = ERREXECString;
    else
        Answer = ACKString;
    return Answer;
}

QString cZDSP1Server::mLoadCmdList(QChar *)
{
    static int count = 0;
    cZDSP1Client* cl = GetClient(ActSock);
    QString errs;
    cl->SetActive(true);
    if (BuildDSProgram(errs)) { // die cmdlisten und die variablen waren schlüssig
        if (!LoadDSProgram()) {
            Answer = ERREXECString;
            cl->SetActive(false);
        }
        else
            Answer = ACKString;
    }
    else {
        cl->SetActive(false);
        Answer = QString("%1 %2").arg(ERRVALString).arg(errs); // das "fehlerhafte" kommando anhängen
    }
    count++;
    qDebug() << QString("LoadCmdList(%1)").arg(count);
    return Answer;
}

QString cZDSP1Server::mSetRavList(QChar *s)
{
    QString qs(s);
    cZDSP1Client* cl = GetClient(ActSock);
    Answer  = cl->SetRavList(qs);
    return Answer;
}

QString cZDSP1Server::mGetRavList()
{
    cZDSP1Client* cl = GetClient(ActSock);
    Answer = cl->GetRavList();
    return Answer;
}

QString cZDSP1Server::mSetCmdIntList(QChar *s)
{
    QString par(s);
    cZDSP1Client* cl = GetClient(ActSock);
    Answer = cl->SetCmdIntListDef(par);
    return Answer;
}

QString cZDSP1Server::mGetCmdIntList()
{
    cZDSP1Client* cl = GetClient(ActSock);
    Answer = cl->GetCmdIntListDef();
    return Answer;
}

QString cZDSP1Server::mSetCmdList(QChar *s)
{
    QString par(s);
    cZDSP1Client* cl = GetClient(ActSock);
    Answer = cl->SetCmdListDef(par);
    return Answer;
}

QString cZDSP1Server::mGetCmdList()
{
    cZDSP1Client* cl = GetClient(ActSock);
    Answer = cl->GetCmdListDef();
    return Answer;
}


QString cZDSP1Server::mMeasure(QChar *s)
{
    QString par(s); // holt den parameter aus dem kommando
    cZDSP1Client* cl = GetClient(ActSock);
    Answer = cl->readActValues(par);
    return Answer;
}

bool cZDSP1Server::setDspType()
{
    int r = readMagicId();
    if ( r == MAGIC_ID21262 ) {
        UserWorkSpaceGlobalSegmentAdr = dm32UserWorkSpaceGlobal21262;
        return m_sDspBootPath.contains("zdsp21262.ldr");
        // adressen im dsp stehen für adsp21262 default richtig
    }
    else
    if ( r == MAGIC_ID21362) {
        UserWorkSpaceGlobalSegmentAdr = dm32UserWorkSpaceGlobal21362;
        if (m_sDspBootPath.contains("zdsp21362.ldr")) {
            // für adsp21362 schreiben wir die adressen um
            dm32DspWorkspace.StartAdr = dm32DspWorkSpaceBase21362;
            dm32DialogWorkSpace.StartAdr = dm32DialogWorkSpaceBase21362;
            dm32UserWorkSpace.StartAdr = dm32UserWorkSpaceBase21362;
            dm32CmdList.StartAdr = dm32CmdListBase21362;

            sDspVar* pDspVar = &CmdListVar;

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

int cZDSP1Server::readMagicId()
{
    return ioctl(m_devFileDescriptor, IO_READ, MagicId);
}

bool cZDSP1Server::Test4HWPresent()
{
    int r = readMagicId();
    return ( (r == MAGIC_ID21262) || (r == MAGIC_ID21362));
}

bool cZDSP1Server::Test4DspRunning()
{
    int r = ioctl(m_devFileDescriptor,IO_READ,DSPStat);
    return ((r & DSP_RUNNING) > 0);
}

QString cZDSP1Server::mDspMemoryRead(QChar* s)
{
    QString par(s);
    cZDSP1Client* cl = GetClient(ActSock);
    Answer = cl->DspVarListRead(par);
    return Answer;
}

QString cZDSP1Server::mDspMemoryWrite(QChar* s)
{
    QString par(s);
    cZDSP1Client* cl = GetClient(ActSock);
    Answer = cl->DspVarWriteRM(par);
    return Answer;
}

cZDSP1Client* cZDSP1Server::GetClient(int s)
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

cZDSP1Client* cZDSP1Server::GetClient(XiQNetPeer *peer)
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

void cZDSP1Server::onEstablishNewConnection(XiQNetPeer *newClient)
{
    connect(newClient, &XiQNetPeer::sigMessageReceived, this, &cZDSP1Server::onExecuteCommandProto);
    connect(newClient, &XiQNetPeer::sigConnectionClosed, this, &cZDSP1Server::deleteConnection);
    AddClient(newClient); // we additionally add the client to our list
}

void cZDSP1Server::deleteConnection()
{
    XiQNetPeer* client = qobject_cast<XiQNetPeer*>(sender());
    DelClient(client);
}

void cZDSP1Server::onExecuteCommandProto(std::shared_ptr<google::protobuf::Message> cmd)
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

            ActSock = m_zdspdClientHash[clientId]->getSocket(); // we set the actual socket (identifier) we have to work on
            QString input = QString::fromStdString(scpiCmd.command()) +  " " + QString::fromStdString(scpiCmd.parameter());
            QString output = pCmdInterpreter->CmdExecute(input);

            ProtobufMessage::NetMessage protobufAnswer;
            ProtobufMessage::NetMessage::NetReply *Answer = protobufAnswer.mutable_reply();

            // dependent on rtype caller can se ack, nak, error
            // in case of error the body has to be analysed for details
            if (output.contains(ACKString))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);
            else
            if (output.contains(NACKString))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_NACK);
            else
            if (output.contains(BUSYString))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (output.contains(ERRVALString))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (output.contains(ERRPATHString))
                Answer->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
            if (output.contains(ERREXECString))
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
            ActSock = GetClient(client)->getSocket();

            QString input =  QString::fromStdString(protobufCommand->scpi().command());
            QString output = pCmdInterpreter->CmdExecute(input);

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

void cZDSP1Server::setSCPIConnection()
{
    m_pSCPISocket = m_pSCPIServer->nextPendingConnection();
    m_pSCPIClient = AddSCPIClient();
    connect(m_pSCPISocket, &QIODevice::readyRead, this, &cZDSP1Server::SCPIInput);
    connect(m_pSCPISocket, &QAbstractSocket::disconnected, this, &cZDSP1Server::SCPIdisconnect);
}

void cZDSP1Server::SCPIInput()
{
    QString m_sInput;
    while ( m_pSCPISocket->canReadLine() )
        m_sInput += m_pSCPISocket->readLine();
    m_sInput.remove('\r'); // we remove cr lf
    m_sInput.remove('\n');
    QString m_sOutput = pCmdInterpreter->CmdExecute(m_sInput) + "\n";
    QByteArray ba = m_sOutput.toLatin1();
    m_pSCPISocket->write(ba);
}

void cZDSP1Server::SCPIdisconnect()
{
    disconnect(m_pSCPISocket, 0, 0, 0); // we disconnect everything
    DelSCPIClient();
}

void cZDSP1Server::SetFASync()
{
    fcntl(m_devFileDescriptor, F_SETOWN, getpid()); // wir sind "besitzer" des device
    int oflags = fcntl(m_devFileDescriptor, F_GETFL);
    fcntl(m_devFileDescriptor, F_SETFL, oflags | FASYNC); // async. benachrichtung (sigio) einschalten
}

cZDSP1Client* cZDSP1Server::AddClient(XiQNetPeer* m_pNetClient)
{
    // fügt einen client hinzu
    m_nSocketIdentifier++;
    if (m_nSocketIdentifier == 0)
        m_nSocketIdentifier++;
    cZDSP1Client* client = new cZDSP1Client(m_nSocketIdentifier, m_pNetClient,this);
    clientlist.append(client);
    return client;
}

void cZDSP1Server::DelClient(XiQNetPeer* netClient)
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

void cZDSP1Server::DelClient(QByteArray clientId)
{
    if (m_zdspdClientHash.contains(clientId)) {
        cZDSP1Client *client = m_zdspdClientHash.take(clientId);
        m_clientIDHash.remove(client);
        clientlist.removeOne(client);
        delete client;
        LoadDSProgram(); // after deleting client we reload dsp program ... means unload dsp for this client
    }
}

cZDSP1Client *cZDSP1Server::AddSCPIClient()
{
    return AddClient(0); // we add this client with netclient (XiQNetPeer) = 0 because it is no XiQNetPeer but
}

void cZDSP1Server::DelSCPIClient()
{
    clientlist.removeAll(m_pSCPIClient);
    LoadDSProgram(); // after deleting client we reload dsp program ... means unload dsp for this client
}

QString cZDSP1Server::SCPICmd(SCPICmdType cmd, QChar *s)
{
    switch ((int)cmd)
    {
    case    TestDsp:            return mTestDsp(s);
    case 	ResetDsp:           return mResetDsp(s);
    case	BootDsp:            return mBootDsp(s);
    case 	SetRavList: 		return mSetRavList(s);
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

QString cZDSP1Server::SCPIQuery( SCPICmdType cmd)
{
    switch ((int)cmd)
    {
    case 		GetPCBSerialNumber: 	return mGetPCBSerialNumber();
    case 		GetDeviceVersion:		return mGetDeviceVersion();
    case 		GetServerVersion: 		return mGetServerVersion();
    case		GetDeviceLoadMax: 	return mGetDeviceLoadMax();
    case 		GetDeviceLoadAct: 	return mGetDeviceLoadAct();
    case		GetDspStatus:		return mGetDspStatus();
    case 		GetDeviceStatus: 		return mGetDeviceStatus();
    case 		GetRavList: 		return mGetRavList();
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
