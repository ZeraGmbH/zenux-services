#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <signal.h>

#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QStringList>
#include <QDebug>
#include <QByteArray>
#include <QCoreApplication>
#include <QSocketNotifier>

#include <xiqnetserver.h>
#include <xmlconfigreader.h>

#include "sec1000dglobal.h"
#include "sec1000d.h"
#include "pcbserver.h"
#include "debugsettings.h"
#include "ethsettings.h"
#include "fpgasettings.h"
#include "ecalcsettings.h"
#include "inputsettings.h"
#include "statusinterface.h"
#include "systeminterface.h"
#include "systeminfo.h"
#include "ecalcinterface.h"
#include "rmconnection.h"

#ifdef SYSTEMD_NOTIFICATION
#include <systemd/sd-daemon.h>
#endif

cSEC1000dServer* SECServer;
int pipeFD[2];
char pipeBuf[2] = "I";
//int anzInt = 0;

void SigHandler(int)
{
    if (SECServer->m_pDebugSettings->getDebugLevel() & 2) syslog(LOG_INFO,"sec interrupt received\n");
    write(pipeFD[1], pipeBuf, 1);
}

struct sigaction mySigAction;
// sigset_t mySigmask, origSigmask;


cSEC1000dServer::cSEC1000dServer()
    :cPCBServer()
{
    m_pDebugSettings = 0;
    m_pETHSettings = 0;
    m_pFPGAsettings = 0;
    m_pECalcSettings = 0;
    m_pInputSettings = 0;
    m_pStatusInterface = 0;
    m_pSystemInterface = 0;
    m_pECalculatorInterface = 0;
    m_pSystemInfo = 0;
    m_pRMConnection = 0;

    m_pInitializationMachine = new QStateMachine(this);

    QState* stateCONF = new QState(); // we start from here
    QFinalState* stateFINISH = new QFinalState(); // and here we finish

    stateCONF->addTransition(this, SIGNAL(abortInit()),stateFINISH); // from anywhere we arrive here if some error

    QState* statexmlConfiguration = new QState(stateCONF); // we configure our server with xml file
    QState* statesetupServer = new QState(stateCONF); // we setup our server now
    stateconnect2RM = new QState(stateCONF); // we connect to resource manager
    stateconnect2RMError = new QState(stateCONF);
    stateSendRMIdentandRegister = new QState(stateCONF); // we send ident. to rm and register our resources
    stateCONF->setInitialState(statexmlConfiguration);

    statexmlConfiguration->addTransition(myXMLConfigReader, SIGNAL(finishedParsingXML(bool)), statesetupServer);
    statesetupServer->addTransition(this, SIGNAL(serverSetup()), stateconnect2RM);
    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);

    QObject::connect(statexmlConfiguration, SIGNAL(entered()), this, SLOT(doConfiguration()));
    QObject::connect(statesetupServer, SIGNAL(entered()), this, SLOT(doSetupServer()));
    QObject::connect(stateconnect2RM, SIGNAL(entered()), this, SLOT(doConnect2RM()));
    QObject::connect(stateconnect2RMError, SIGNAL(entered()), this, SLOT(connect2RMError()));
    QObject::connect(stateSendRMIdentandRegister, SIGNAL(entered()), this, SLOT(doIdentAndRegister()));
    QObject::connect(stateFINISH, SIGNAL(entered()), this, SLOT(doCloseServer()));

    m_pInitializationMachine->start();
}


cSEC1000dServer::~cSEC1000dServer()
{
    if (m_pDebugSettings) delete m_pDebugSettings;
    if (m_pETHSettings) delete m_pETHSettings;
    if (m_pFPGAsettings) delete m_pFPGAsettings;
    if (m_pECalcSettings) delete m_pECalcSettings;
    if (m_pInputSettings) delete m_pInputSettings;
    if (m_pStatusInterface) delete m_pStatusInterface;
    if (m_pSystemInterface) delete m_pSystemInterface;
    if (m_pECalculatorInterface) delete m_pECalculatorInterface;
    if (m_pSystemInfo) delete m_pSystemInfo;
    if (m_pRMConnection) delete m_pRMConnection;

    close(DevFileDescriptor); // close dev.
    close(pipeFD[0]);
    close(pipeFD[1]);
}


void cSEC1000dServer::doConfiguration()
{
    QStringList args;

    args = QCoreApplication::instance()->arguments();
    if (args.count() != 2) // we want exactly 1 parameter
    {
        m_nerror = parameterError;
        emit abortInit();
    }
    else
    {
        if ( pipe(pipeFD) == -1 )
        {
            m_nerror = pipeError;
            emit abortInit();
        }
        else
        {
            fcntl( pipeFD[1], F_SETFL, O_NONBLOCK);
            fcntl( pipeFD[0], F_SETFL, O_NONBLOCK);
            m_pNotifier = new QSocketNotifier(pipeFD[0], QSocketNotifier::Read, this);
            connect(m_pNotifier, SIGNAL(activated(int)), this, SLOT(SECIntHandler(int)));
            if (myXMLConfigReader->loadSchema(defaultXSDFile))
            {
                // we want to initialize all settings first
                m_pDebugSettings = new cDebugSettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pDebugSettings,SLOT(configXMLInfo(const QString&)));
                m_pETHSettings = new cETHSettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pETHSettings,SLOT(configXMLInfo(const QString&)));
                m_pFPGAsettings = new cFPGASettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pFPGAsettings,SLOT(configXMLInfo(const QString&)));
                m_pECalcSettings = new cECalculatorSettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pECalcSettings,SLOT(configXMLInfo(const QString&)));
                m_pInputSettings = new cInputSettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pInputSettings,SLOT(configXMLInfo(const QString&)));

                QString s = args.at(1);
                qDebug() << s;

                if(!myXMLConfigReader->loadXMLFile(s)) {
                    m_nerror = xmlfileError;
                    emit abortInit();
                }
            }
            else
            {
                m_nerror = xsdfileError;
                emit abortInit();
            }
        }

    }
}


void cSEC1000dServer::doSetupServer()
{

    m_sSECDeviceNode = m_pFPGAsettings->getDeviceNode(); // we try to open the sec device

    if (SECDevOpen() < 0)
    {
        m_nerror = secDeviceError; // and finish if not possible
        emit abortInit();
    }
    else
    {
        m_pSystemInfo = new cSystemInfo();

        cPCBServer::setupServer(); // here our scpi interface gets instanciated, we need this for further steps

        scpiConnectionList.append(this); // the server itself has some commands
        scpiConnectionList.append(m_pStatusInterface = new cStatusInterface());
        scpiConnectionList.append(m_pSystemInterface = new cSystemInterface(this, m_pSystemInfo));
        scpiConnectionList.append(m_pECalculatorInterface = new cECalculatorInterface(this, m_pETHSettings, m_pECalcSettings, m_pFPGAsettings, m_pInputSettings));

        resourceList.append(m_pECalculatorInterface); // all our resources
        m_ECalculatorChannelList = m_pECalculatorInterface->getECalcChannelList(); // we use this list in interrupt service

        SECServer = this;
        m_nDebugLevel = m_pDebugSettings->getDebugLevel();

        initSCPIConnections();

        myServer->startServer(m_pETHSettings->getPort(server)); // and can start the server now

        mySigAction.sa_handler = &SigHandler; // signal handler einrichten
        sigemptyset(&mySigAction.sa_mask);
        mySigAction. sa_flags = SA_RESTART;
        mySigAction.sa_restorer = NULL;
        sigaction(SIGIO, &mySigAction, NULL); // handler für sigio definieren
        SetFASync();
        // our resource mananager connection must be opened after configuration is done
        m_pRMConnection = new cRMConnection(m_pETHSettings->getRMIPadr(), m_pETHSettings->getPort(resourcemanager), m_pDebugSettings->getDebugLevel());
        //connect(m_pRMConnection, SIGNAL(connectionRMError()), this, SIGNAL(abortInit()));
        // so we must complete our state machine here
        m_nRetryRMConnect = 100;
        m_retryTimer.setSingleShot(true);
        connect(&m_retryTimer, SIGNAL(timeout()), this, SIGNAL(serverSetup()));

        stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connected()), stateSendRMIdentandRegister);
        stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connectionRMError()), stateconnect2RMError);
        stateconnect2RMError->addTransition(this, SIGNAL(serverSetup()), stateconnect2RM);

        emit serverSetup(); // so we enter state machine's next state
    }
}


void cSEC1000dServer::doCloseServer()
{
    QCoreApplication::instance()->exit(m_nerror);
}


void cSEC1000dServer::doConnect2RM()
{
    m_nerror = rmConnectionError; // preset error condition
    m_pRMConnection->connect2RM();
}


void cSEC1000dServer::connect2RMError()
{
    m_nRetryRMConnect--;
    if (m_nRetryRMConnect == 0)
        emit abortInit();
    else
        m_retryTimer.start(200);
}


void cSEC1000dServer::doIdentAndRegister()
{
    m_pRMConnection->SendIdent(getName());

    for (int i = 0; i < resourceList.count(); i++)
    {
        cResource *res = resourceList.at(i);
        connect(m_pRMConnection, SIGNAL(rmAck(quint32)), res, SLOT(resourceManagerAck(quint32)) );
        res->registerResource(m_pRMConnection, m_pETHSettings->getPort(server));
    }

#ifdef SYSTEMD_NOTIFICATION
    sd_notify(0, "READY=1");
#endif

}


int cSEC1000dServer::SECDevOpen()
{
    if ( (DevFileDescriptor = open(m_sSECDeviceNode.toLatin1().data(), O_RDWR)) < 0 )
    {
        if (DEBUG1)  syslog(LOG_ERR,"error opening sec device: %s\n",m_pFPGAsettings->getDeviceNode().toLatin1().data());
    }
    return DevFileDescriptor;
}


void cSEC1000dServer::SetFASync()
{
    fcntl(DevFileDescriptor, F_SETOWN, getpid()); // wir sind "besitzer" des device
    int oflags = fcntl(DevFileDescriptor, F_GETFL);
    fcntl(DevFileDescriptor, F_SETFL, oflags | FASYNC); // async. benachrichtung (sigio) einschalten
}


void cSEC1000dServer::SECIntHandler(int)
{ // behandelt den sec interrupt

    char buf[2];

    read(pipeFD[0], buf, 1); // first we read the pipe

    int n = m_ECalculatorChannelList.count(); // the number of error calculator entities
    // 8 error calc entities share 1 32 bit data word for interrupts

    n /= 2; // so we have to read 4 bytes for 8 entities -> (/ 2)
    QByteArray interruptREGS(n, 0);
    // first word is interrupt collection word
    lseek(DevFileDescriptor, m_pECalcSettings->getIrqAdress()+4, 0); // so the dedicated words have +4 offset
    read(DevFileDescriptor, interruptREGS.data(), n);

    for (int i = 0; i < n; i++)
    {
        quint8 irq = interruptREGS[i];
        m_ECalculatorChannelList.at(i*2)->setIntReg(irq & 0xF); // this will cause notifier to be thrown
        m_ECalculatorChannelList.at(i*2+1)->setIntReg((irq >> 4) & 0xF);
    }
}
