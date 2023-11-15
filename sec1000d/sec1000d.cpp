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
#include "debugsettings.h"
#include "ethsettings.h"
#include "fpgasettings.h"
#include "seccalcsettings.h"
#include "secinputsettings.h"
#include "statusinterface.h"
#include "systeminterface.h"
#include "systeminfo.h"
#include "secgroupresourceandinterface.h"
#include "rmconnection.h"
#include <scpisingletonfactory.h>

#ifdef SYSTEMD_NOTIFICATION
#include <systemd/sd-daemon.h>
#endif

cSEC1000dServer* SECServer;

int pipeFD[2];

void SigHandler(int)
{
    write(pipeFD[1], "I", 1);
}

struct sigaction mySigAction;
// sigset_t mySigmask, origSigmask;
static ServerParams params {ServerName, ServerVersion, "/etc/zera/sec1000d/sec1000d.xsd", "/etc/zera/sec1000d/sec1000d.xml"};

cSEC1000dServer::cSEC1000dServer() :
    cPCBServer(params, ScpiSingletonFactory::getScpiObj())
{
    m_pDebugSettings = 0;
    m_pFPGASettings = 0;
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
    m_stateconnect2RM = new QState(stateCONF); // we connect to resource manager
    m_stateconnect2RMError = new QState(stateCONF);
    m_stateSendRMIdentAndRegister = new QState(stateCONF); // we send ident. to rm and register our resources
    stateCONF->setInitialState(statexmlConfiguration);

    statexmlConfiguration->addTransition(&m_xmlConfigReader, SIGNAL(finishedParsingXML(bool)), statesetupServer);
    statesetupServer->addTransition(this, SIGNAL(sigServerIsSetUp()), m_stateconnect2RM);
    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);

    QObject::connect(statexmlConfiguration, &QAbstractState::entered, this, &cSEC1000dServer::doConfiguration);
    QObject::connect(statesetupServer, &QAbstractState::entered, this, &cSEC1000dServer::doSetupServer);
    QObject::connect(m_stateconnect2RM, &QAbstractState::entered, this, &cSEC1000dServer::doConnect2RM);
    QObject::connect(m_stateconnect2RMError, &QAbstractState::entered, this, &cSEC1000dServer::connect2RMError);
    QObject::connect(m_stateSendRMIdentAndRegister, &QAbstractState::entered, this, &cSEC1000dServer::doIdentAndRegister);
    QObject::connect(stateFINISH, &QAbstractState::entered, this, &cSEC1000dServer::doCloseServer);

    m_pInitializationMachine->start();
}


cSEC1000dServer::~cSEC1000dServer()
{
    if (m_pDebugSettings) delete m_pDebugSettings;
    if (m_pFPGASettings) delete m_pFPGASettings;
    if (m_pECalcSettings) delete m_pECalcSettings;
    if (m_pInputSettings) delete m_pInputSettings;
    if (m_pStatusInterface) delete m_pStatusInterface;
    if (m_pSystemInterface) delete m_pSystemInterface;
    if (m_pECalculatorInterface) delete m_pECalculatorInterface;
    if (m_pSystemInfo) delete m_pSystemInfo;
    if (m_pRMConnection) delete m_pRMConnection;

    close(m_devFileDescriptor); // close dev.
    close(pipeFD[0]);
    close(pipeFD[1]);
}


void cSEC1000dServer::doConfiguration()
{
    if ( pipe(pipeFD) == -1 ) {
        m_nerror = pipeError;
        emit abortInit();
    }
    else {
        fcntl( pipeFD[1], F_SETFL, O_NONBLOCK);
        fcntl( pipeFD[0], F_SETFL, O_NONBLOCK);
        m_pNotifier = new QSocketNotifier(pipeFD[0], QSocketNotifier::Read, this);
        connect(m_pNotifier, &QSocketNotifier::activated, this, &cSEC1000dServer::SECIntHandler);
        if (m_xmlConfigReader.loadSchema(params.xsdFile)) {
            // we want to initialize all settings first
            m_pDebugSettings = new cDebugSettings(&m_xmlConfigReader);
            connect(&m_xmlConfigReader,&Zera::XMLConfig::cReader::valueChanged,m_pDebugSettings,&cDebugSettings::configXMLInfo);
            connect(&m_xmlConfigReader,&Zera::XMLConfig::cReader::valueChanged,&m_ethSettings,&EthSettings::configXMLInfo);
            m_pFPGASettings = new FPGASettings(&m_xmlConfigReader);
            connect(&m_xmlConfigReader,&Zera::XMLConfig::cReader::valueChanged,m_pFPGASettings,&FPGASettings::configXMLInfo);
            m_pECalcSettings = new SecCalculatorSettings(&m_xmlConfigReader);
            connect(&m_xmlConfigReader,&Zera::XMLConfig::cReader::valueChanged,m_pECalcSettings,&SecCalculatorSettings::configXMLInfo);
            m_pInputSettings = new SecInputSettings(&m_xmlConfigReader);
            connect(&m_xmlConfigReader,&Zera::XMLConfig::cReader::valueChanged,m_pInputSettings,&SecInputSettings::configXMLInfo);

            if(!m_xmlConfigReader.loadXMLFile(params.xmlFile)) {
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

void cSEC1000dServer::doSetupServer()
{
    m_sSECDeviceNode = m_pFPGASettings->getDeviceNode(); // we try to open the sec device
    if (SECDevOpen() < 0)
    {
        m_nerror = secDeviceError; // and finish if not possible
        emit abortInit();
    }
    else
    {
        m_pSystemInfo = new cSystemInfo();

        setupServer(); // here our scpi interface gets instanciated, we need this for further steps

        scpiConnectionList.append(this); // the server itself has some commands
        scpiConnectionList.append(m_pStatusInterface = new cStatusInterface());
        scpiConnectionList.append(m_pSystemInterface = new cSystemInterface(this, m_pSystemInfo));
        scpiConnectionList.append(m_pECalculatorInterface = new SecGroupResourceAndInterface(m_devFileDescriptor,
                                                                                             m_pECalcSettings,
                                                                                             m_pInputSettings,
                                                                                             SigHandler));

        resourceList.append(m_pECalculatorInterface); // all our resources
        m_ECalculatorChannelList = m_pECalculatorInterface->getECalcChannelList(); // we use this list in interrupt service

        SECServer = this;

        initSCPIConnections();

        m_myServer->startServer(m_ethSettings.getPort(EthSettings::protobufserver)); // and can start the server now

        mySigAction.sa_handler = &SigHandler; // signal handler einrichten
        sigemptyset(&mySigAction.sa_mask);
        mySigAction. sa_flags = SA_RESTART;
        mySigAction.sa_restorer = NULL;
        sigaction(SIGIO, &mySigAction, NULL); // handler für sigio definieren
        SetFASync();
        // our resource mananager connection must be opened after configuration is done
        m_pRMConnection = new RMConnection(m_ethSettings.getRMIPadr(), m_ethSettings.getPort(EthSettings::resourcemanager));
        //connect(m_pRMConnection, SIGNAL(connectionRMError()), this, SIGNAL(abortInit()));
        // so we must complete our state machine here
        m_retryRMConnect = 100;
        m_retryTimer.setSingleShot(true);
        connect(&m_retryTimer, &QTimer::timeout, this, &cSEC1000dServer::sigServerIsSetUp);

        m_stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connected()), m_stateSendRMIdentAndRegister);
        m_stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connectionRMError()), m_stateconnect2RMError);
        m_stateconnect2RMError->addTransition(this, SIGNAL(sigServerIsSetUp()), m_stateconnect2RM);

        emit sigServerIsSetUp(); // so we enter state machine's next state
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
    m_retryRMConnect--;
    if (m_retryRMConnect == 0)
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
        res->registerResource(m_pRMConnection, m_ethSettings.getPort(EthSettings::protobufserver));
    }

#ifdef SYSTEMD_NOTIFICATION
    sd_notify(0, "READY=1");
#endif
}

int cSEC1000dServer::SECDevOpen()
{
    m_devFileDescriptor = open(m_sSECDeviceNode.toLatin1().data(), O_RDWR);
    if (m_devFileDescriptor < 0)
        qWarning("Error opening sec device: %s", qPrintable(m_pFPGASettings->getDeviceNode()));
    return m_devFileDescriptor;
}

void cSEC1000dServer::SetFASync()
{
    fcntl(m_devFileDescriptor, F_SETOWN, getpid()); // wir sind "besitzer" des device
    int oflags = fcntl(m_devFileDescriptor, F_GETFL);
    fcntl(m_devFileDescriptor, F_SETFL, oflags | FASYNC); // async. benachrichtung (sigio) einschalten
}

void cSEC1000dServer::SECIntHandler(int)
{ // behandelt den sec interrupt

    char dummy[2];
    read(pipeFD[0], dummy, 1); // first we read the pipe

    int n = m_ECalculatorChannelList.count(); // the number of error calculator entities
    // 8 error calc entities share 1 32 bit data word for interrupts

    n /= 2; // so we have to read 4 bytes for 8 entities -> (/ 2)
    QByteArray interruptREGS(n, 0);
    // first word is interrupt collection word
    lseek(m_devFileDescriptor, m_pECalcSettings->getIrqAdress()+4, 0); // so the dedicated words have +4 offset
    read(m_devFileDescriptor, interruptREGS.data(), n);

    for (int i = 0; i < n; i++) {
        quint8 irq = interruptREGS[i];
        m_ECalculatorChannelList.at(i*2)->setIntReg(irq & 0xF); // this will cause notifier to be thrown
        m_ECalculatorChannelList.at(i*2+1)->setIntReg((irq >> 4) & 0xF);
    }
}
