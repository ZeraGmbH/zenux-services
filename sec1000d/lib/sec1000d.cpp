#include "secdevicenodesingleton.h"
#include "sec1000d.h"
#include "debugsettings.h"
#include "ethsettings.h"
#include "fpgasettings.h"
#include "seccalcsettings.h"
#include "secinputsettings.h"
#include "sec1000statusinterface.h"
#include "sec1000systeminterface.h"
#include "sec1000systeminfo.h"
#include "secgroupresourceandinterface.h"
#include "rmconnection.h"
#include <scpisingletonfactory.h>
#include <xiqnetserver.h>
#include <xmlconfigreader.h>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QStringList>
#include <QDebug>
#include <QByteArray>
#include <QCoreApplication>
#include <QSocketNotifier>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#ifdef SYSTEMD_NOTIFICATION
#include <systemd/sd-daemon.h>
#endif

#define ServerName "sec1000d"
#define ServerVersion "V1.00"

cSEC1000dServer* SECServer;

int pipeFileDescriptorSec1000[2];
static void SigHandler(int)
{
    const static char pipeFDBuf[2] = "I";
    write(pipeFileDescriptorSec1000[1], pipeFDBuf, 1);
}

struct sigaction sigActionSec1000;
// sigset_t mySigmask, origSigmask;


ServerParams cSEC1000dServer::defaultParams{ServerName, ServerVersion, "/etc/zera/sec1000d/sec1000d.xsd", "/etc/zera/sec1000d/sec1000d.xml"};

cSEC1000dServer::cSEC1000dServer(ServerParams params) :
    cPCBServer(params, ScpiSingletonFactory::getScpiObj()),
    m_params(params)
{
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
    delete m_pDebugSettings;
    delete m_pFPGASettings;
    delete m_pECalcSettings;
    delete m_pInputSettings;
    delete m_pStatusInterface;
    delete m_pSystemInterface;
    delete m_pECalculatorInterface;
    delete m_pSystemInfo;
    delete m_pRMConnection;

    SecDeviceNodeSingleton::getInstance()->close();
    close(pipeFileDescriptorSec1000[0]);
    close(pipeFileDescriptorSec1000[1]);
}

QString cSEC1000dServer::getServerVersion()
{
    return QString("%1 %2").arg(ServerName, ServerVersion);;
}

void cSEC1000dServer::doConfiguration()
{
    if ( pipe(pipeFileDescriptorSec1000) == -1 ) {
        qCritical("Abort, could not open pipe");
        emit abortInit();
    }
    else {
        fcntl( pipeFileDescriptorSec1000[1], F_SETFL, O_NONBLOCK);
        fcntl( pipeFileDescriptorSec1000[0], F_SETFL, O_NONBLOCK);
        m_pNotifier = new QSocketNotifier(pipeFileDescriptorSec1000[0], QSocketNotifier::Read, this);
        connect(m_pNotifier, &QSocketNotifier::activated, this, &cSEC1000dServer::SECIntHandler);
        if (m_xmlConfigReader.loadSchema(m_params.xsdFile)) {
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

            if(!m_xmlConfigReader.loadXMLFile(m_params.xmlFile)) {
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

void cSEC1000dServer::doSetupServer()
{
    QString deviceNodeName = m_pFPGASettings->getDeviceNode(); // we try to open the sec device
    if (SecDeviceNodeSingleton::getInstance()->open(deviceNodeName) < 0) {
        qCritical("Abort, could not poen device node %s", qPrintable(deviceNodeName));
        emit abortInit();
    }
    else
    {
        m_pSystemInfo = new Sec1000SystemInfo();

        setupServer(); // here our scpi interface gets instanciated, we need this for further steps

        scpiConnectionList.append(this); // the server itself has some commands
        scpiConnectionList.append(m_pStatusInterface = new Sec1000StatusInterface());
        scpiConnectionList.append(m_pSystemInterface = new cSystemInterface(this, m_pSystemInfo));
        scpiConnectionList.append(m_pECalculatorInterface = new SecGroupResourceAndInterface(m_pECalcSettings,
                                                                                             m_pInputSettings,
                                                                                             SigHandler));

        resourceList.append(m_pECalculatorInterface); // all our resources
        m_ECalculatorChannelList = m_pECalculatorInterface->getECalcChannelList(); // we use this list in interrupt service

        SECServer = this;

        initSCPIConnections();

        m_myServer->startServer(m_ethSettings.getPort(EthSettings::protobufserver)); // and can start the server now

        sigActionSec1000.sa_handler = &SigHandler; // signal handler einrichten
        sigemptyset(&sigActionSec1000.sa_mask);
        sigActionSec1000. sa_flags = SA_RESTART;
        sigActionSec1000.sa_restorer = NULL;
        sigaction(SIGIO, &sigActionSec1000, NULL); // handler für sigio definieren
        SecDeviceNodeSingleton::getInstance()->enableFasync();
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

void cSEC1000dServer::onPeerDisconnected()
{
    XiQNetPeer* peer = qobject_cast<XiQNetPeer*>(sender());
    if(!m_pECalculatorInterface->freeChannelsForThisPeer(peer))
        qWarning("Client disconnected. But SEC resources could not be freed!");
}

void cSEC1000dServer::SECIntHandler(int)
{ // behandelt den sec interrupt

    char dummy[2];
    read(pipeFileDescriptorSec1000[0], dummy, 1); // first we read the pipe

    int n = m_ECalculatorChannelList.count(); // the number of error calculator entities
    // 8 error calc entities share 1 32 bit data word for interrupts

    n /= 2; // so we have to read 4 bytes for 8 entities -> (/ 2)
    QByteArray interruptREGS(n, 0);
    // first word is interrupt collection word
    SecDeviceNodeSingleton::getInstance()->lseek(m_pECalcSettings->getIrqAdress()+4); // so the dedicated words have +4 offset
    SecDeviceNodeSingleton::getInstance()->read(interruptREGS.data(), n);

    for (int i = 0; i < n; i++) {
        quint8 irq = interruptREGS[i];
        m_ECalculatorChannelList.at(i*2)->setIntReg(irq & 0xF); // this will cause notifier to be thrown
        m_ECalculatorChannelList.at(i*2+1)->setIntReg((irq >> 4) & 0xF);
    }
}
