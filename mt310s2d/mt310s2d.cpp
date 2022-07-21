#include "mt310s2d.h"
#include "mt310s2dglobal.h"
#include "systeminfo.h"
#include "adjustment.h"
#include "rmconnection.h"
#include "micro-controller-io/atmelsysctrl.h"
#include "micro-controller-io/atmel.h"
#include "micro-controller-io/atmelwatcher.h"
#include "scpi-interfaces/clampinterface.h"
#include "scpi-interfaces/frqinputinterface.h"
#include "scpi-interfaces/hkeyinterface.h"
#include "scpi-interfaces/samplinginterface.h"
#include "scpi-interfaces/scheadinterface.h"
#include "scpi-interfaces/senseinterface.h"
#include "scpi-interfaces/sourceinterface.h"
#include "scpi-interfaces/statusinterface.h"
#include "scpi-interfaces/systeminterface.h"
#include "settings/ctrlsettings.h"
#include "settings/debugsettings.h"
#include "settings/ethsettings.h"
#include "settings/frqinputsettings.h"
#include "settings/fpgasettings.h"
#include "settings/hkeysettings.h"
#include "settings/i2csettings.h"
#include "settings/samplingsettings.h"
#include "settings/scheadsettings.h"
#include "settings/sensesettings.h"
#include "settings/sourcesettings.h"
#include <xmlconfigreader.h>
#include <xiqnetserver.h>
#include <QSocketNotifier>
#include <QCoreApplication>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <signal.h>

#ifdef SYSTEMD_NOTIFICATION
#include <systemd/sd-daemon.h>
#endif


static cMT310S2dServer* MTServer;
static int pipeFD[2];
static char pipeFDBuf[2] = "I";

void SigHandler(int)
{
    if (MTServer->m_pDebugSettings->getDebugLevel() & 2) syslog(LOG_INFO,"mt interrupt received\n");
        write(pipeFD[1], pipeFDBuf, 1);
}


static struct sigaction mySigAction;
// sigset_t mySigmask, origSigmask;


cATMELSysCtrl* pAtmelSys; // we take a static object for atmel connection
cATMEL* pAtmel; // we take a static object for atmel connection

cMT310S2dServer::cMT310S2dServer()
{
    // TODO: Move to cPCBServer::cPCBServer?
    m_pDebugSettings = nullptr;
    m_pETHSettings = nullptr;
    m_pI2CSettings = nullptr;
    m_pFPGASettings = nullptr;
    m_pCtrlSettings  = nullptr;
    m_pSenseSettings = nullptr;
    pAtmelSys = nullptr;
    pAtmel = nullptr;
    m_pAtmelWatcher = nullptr;
    m_pStatusInterface = nullptr;
    m_pSystemInterface = nullptr;
    m_pSenseInterface = nullptr;
    m_pClampInterface = nullptr;
    m_pSystemInfo = nullptr;
    m_pAdjHandler = nullptr;
    m_pRMConnection = nullptr;

    m_pInitializationMachine = new QStateMachine(this);

    QState* stateCONF = new QState(); // we start from here
    QFinalState* stateFINISH = new QFinalState(); // and here we finish

    stateCONF->addTransition(this, SIGNAL(abortInit()),stateFINISH); // from anywhere we arrive here if some error

    QState* statexmlConfiguration = new QState(stateCONF); // we configure our server with xml file
    QState* statewait4Atmel = new QState(stateCONF); // we synchronize on atmel running
    QState* statesetupServer = new QState(stateCONF); // we setup our server now
    stateconnect2RM = new QState(stateCONF); // we connect to resource manager
    stateconnect2RMError = new QState(stateCONF);
    stateSendRMIdentandRegister = new QState(stateCONF); // we send ident. to rm and register our resources

    stateCONF->setInitialState(statexmlConfiguration);

    statexmlConfiguration->addTransition(myXMLConfigReader, SIGNAL(finishedParsingXML(bool)), statewait4Atmel);
    statewait4Atmel->addTransition(this, SIGNAL(atmelRunning()), statesetupServer);
    statesetupServer->addTransition(this, SIGNAL(serverSetup()), stateconnect2RM);

    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);

    QObject::connect(statexmlConfiguration, SIGNAL(entered()), this, SLOT(doConfiguration()));
    QObject::connect(statewait4Atmel, SIGNAL(entered()), this, SLOT(doWait4Atmel()));
    QObject::connect(statesetupServer, SIGNAL(entered()), this, SLOT(doSetupServer()));
    QObject::connect(stateconnect2RM, SIGNAL(entered()), this, SLOT(doConnect2RM()));
    QObject::connect(stateconnect2RMError, SIGNAL(entered()), this, SLOT(connect2RMError()));
    QObject::connect(stateSendRMIdentandRegister, SIGNAL(entered()), this, SLOT(doIdentAndRegister()));
    QObject::connect(stateFINISH, SIGNAL(entered()), this, SLOT(doCloseServer()));

    m_pInitializationMachine->start();
}


cMT310S2dServer::~cMT310S2dServer()
{
    if (m_pDebugSettings) delete m_pDebugSettings;
    if (m_pETHSettings) delete m_pETHSettings;
    if (m_pI2CSettings) delete m_pI2CSettings;
    if (m_pFPGASettings) delete m_pFPGASettings;
    if (m_pCtrlSettings) delete m_pCtrlSettings;
    if (m_pSenseSettings) delete m_pSenseSettings;
    if (m_pSourceSettings) delete m_pSourceSettings;
    if (m_pFRQInputSettings) delete m_pFRQInputSettings;
    if (m_pSCHeadSettings) delete m_pSCHeadSettings;
    if (pAtmelSys) delete pAtmelSys;
    if (pAtmel) delete pAtmel;
    if (m_pAtmelWatcher) delete m_pAtmelWatcher;
    if (m_pStatusInterface) delete m_pStatusInterface;
    if (m_pSystemInterface) delete m_pSystemInterface;
    if (m_pSenseInterface) delete m_pSenseInterface;
    if (m_pClampInterface) delete m_pClampInterface;
    if (m_pSourceInterface) delete m_pSourceInterface;
    if (m_pFRQInputInterface) delete m_pFRQInputInterface;
    if (m_pSCHeadInterface) delete m_pSCHeadInterface;
    if (m_pHKeyInterface) delete m_pHKeyInterface;
    if (m_pSystemInfo) delete m_pSystemInfo;
    if (m_pAdjHandler) delete m_pAdjHandler;
    if (m_pRMConnection) delete m_pRMConnection;
}

void cMT310S2dServer::doConfiguration()
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
        if ( pipe(pipeFD) == -1)
        {
            m_nerror = pipeError;
            emit abortInit();
        }
        else
        {
            fcntl( pipeFD[1], F_SETFL, O_NONBLOCK);
            fcntl( pipeFD[0], F_SETFL, O_NONBLOCK);
            m_pNotifier = new QSocketNotifier(pipeFD[0], QSocketNotifier::Read, this);
            connect(m_pNotifier, SIGNAL(activated(int)), this, SLOT(MTIntHandler(int)));

            if (myXMLConfigReader->loadSchema(defaultXSDFile))
            {

                // we want to initialize all settings first
                m_pDebugSettings = new cDebugSettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pDebugSettings,SLOT(configXMLInfo(const QString&)));
                m_pETHSettings = new cETHSettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pETHSettings,SLOT(configXMLInfo(const QString&)));
                m_pI2CSettings = new cI2CSettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pI2CSettings,SLOT(configXMLInfo(const QString&)));
                m_pFPGASettings = new cFPGASettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pFPGASettings,SLOT(configXMLInfo(const QString&)));
                m_pCtrlSettings = new cCtrlSettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pCtrlSettings,SLOT(configXMLInfo(const QString&)));
                m_pSenseSettings = new cSenseSettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pSenseSettings,SLOT(configXMLInfo(const QString&)));
                m_pSourceSettings = new cSourceSettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pSourceSettings,SLOT(configXMLInfo(const QString&)));
                m_pSamplingSettings = new cSamplingSettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pSamplingSettings,SLOT(configXMLInfo(const QString&)));
                m_pFRQInputSettings = new cFRQInputSettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pFRQInputSettings,SLOT(configXMLInfo(const QString&)));
                m_pSCHeadSettings = new cSCHeadSettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pSCHeadSettings,SLOT(configXMLInfo(const QString&)));
                m_pHKeySettings = new cHKeySettings(myXMLConfigReader);
                connect(myXMLConfigReader,SIGNAL(valueChanged(const QString&)),m_pHKeySettings,SLOT(configXMLInfo(const QString&)));


                QString s = args.at(1);
                qDebug() << s;

                if (!myXMLConfigReader->loadXMLFile(s)) {
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


void cMT310S2dServer::doWait4Atmel()
{
    // a singletom for atmel would be nice...
    pAtmelSys = new cATMELSysCtrl(m_pI2CSettings->getDeviceNode(), m_pI2CSettings->getI2CAdress(i2cSettings::atmelsys), m_pDebugSettings->getDebugLevel());
    pAtmel = new cATMEL(m_pI2CSettings->getDeviceNode(), m_pI2CSettings->getI2CAdress(i2cSettings::atmel), m_pDebugSettings->getDebugLevel());
    m_pAtmelWatcher = new cAtmelWatcher(m_pDebugSettings->getDebugLevel(), m_pCtrlSettings->getDeviceNode(), 10000, 100);

    m_nerror = atmelError; // we preset error
    connect(m_pAtmelWatcher,SIGNAL(timeout()),this,SIGNAL(abortInit()));
    connect(m_pAtmelWatcher,SIGNAL(running()),this,SIGNAL(atmelRunning()));
    m_pAtmelWatcher->start();
}


void cMT310S2dServer::doSetupServer()
{
    m_sCtrlDeviceNode = m_pCtrlSettings->getDeviceNode(); // we try to open the ctrl device
    m_sMessageDeviceNode = m_pFPGASettings->getDeviceNode();

    MTServer = this;

    if (CtrlDevOpen() < 0)
    {
        m_nerror = ctrlDeviceError; // and finish if not possible
        emit abortInit();
    }
    else
        if (MessageDevOpen() < 0)
        {
            m_nerror = fpgaDeviceError;
            emit abortInit();
        }
        else
        {
            pAtmel->setPLLChannel(1); // default channel m0 for pll control
            m_pSystemInfo = new cSystemInfo();
            m_pAdjHandler = new cAdjustment(this);

            //m_pSystemInfo, m_pI2CSettings->getDeviceNode(), m_pDebugSettings->getDebugLevel(), m_pI2CSettings->getI2CAdress(i2cSettings::flash)

            cPCBServer::setupServer(); // here our scpi interface gets instanciated, we need this for further steps

            scpiConnectionList.append(this); // the server itself has some commands
            scpiConnectionList.append(m_pStatusInterface = new cStatusInterface(this));
            scpiConnectionList.append(m_pSystemInterface = new cSystemInterface(this));
            scpiConnectionList.append(m_pSenseInterface = new cSenseInterface(this));
            scpiConnectionList.append(m_pSamplingInterface = new cSamplingInterface(this));
            scpiConnectionList.append(m_pSourceInterface = new cSourceInterface(this));
            scpiConnectionList.append(m_pFRQInputInterface = new cFRQInputInterface(this));
            scpiConnectionList.append(m_pSCHeadInterface = new cSCHeadInterface(this));
            scpiConnectionList.append(m_pHKeyInterface = new cHKeyInterface(this));
            scpiConnectionList.append(m_pClampInterface = new cClampInterface(this));

            resourceList.append(m_pSenseInterface); // all our resources
            resourceList.append(m_pSamplingInterface);
            resourceList.append(m_pSourceInterface);
            resourceList.append(m_pFRQInputInterface);
            resourceList.append(m_pSCHeadInterface);
            resourceList.append(m_pHKeyInterface);


            m_pAdjHandler->addAdjFlashObject(m_pSenseInterface);
            m_pSenseInterface->importAdjFlash(); // we read adjustmentdata at least once

            initSCPIConnections();

            // after init. we once poll the devices connected at power up
            updateI2cDevicesConnected();

            myServer->startServer(m_pETHSettings->getPort(protobufserver)); // and can start the server now
            m_pSCPIServer->listen(QHostAddress::AnyIPv4, m_pETHSettings->getPort(scpiserver));

            mySigAction.sa_handler = &SigHandler; // setup signal handler
            sigemptyset(&mySigAction.sa_mask);
            mySigAction. sa_flags = SA_RESTART;
            mySigAction.sa_restorer = nullptr;
            sigaction(SIGIO, &mySigAction, nullptr); // set handler for sigio

            SetFASync();
            enableClampInterrupt();

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


void cMT310S2dServer::doCloseServer()
{
    QCoreApplication::instance()->exit(m_nerror);
}


void cMT310S2dServer::doConnect2RM()
{
    m_nerror = rmConnectionError; // preset error condition
    m_pRMConnection->connect2RM();
}


void cMT310S2dServer::connect2RMError()
{
    m_nRetryRMConnect--;
    if (m_nRetryRMConnect == 0)
        emit abortInit();
    else
        m_retryTimer.start(200);
}


void cMT310S2dServer::doIdentAndRegister()
{
    m_pRMConnection->SendIdent(getName());

    for (int i = 0; i < resourceList.count(); i++)
    {
        cResource *res = resourceList.at(i);
        connect(m_pRMConnection, SIGNAL(rmAck(quint32)), res, SLOT(resourceManagerAck(quint32)) );
        res->registerResource(m_pRMConnection, m_pETHSettings->getPort(protobufserver));
    }
#ifdef SYSTEMD_NOTIFICATION
    sd_notify(0, "READY=1");
#endif
}


int cMT310S2dServer::CtrlDevOpen()
{
    if ( (DevFileDescriptorCtrl = open(m_sCtrlDeviceNode.toLatin1().data(), O_RDWR)) < 0 )
    {
        if (m_pDebugSettings->getDebugLevel() & 1)  syslog(LOG_ERR,"error opening ctrl device: %s\n",m_pCtrlSettings->getDeviceNode().toLatin1().data());
    }
    return DevFileDescriptorCtrl;
}


int cMT310S2dServer::MessageDevOpen()
{
    if ( (DevFileDescriptorMsg = open(m_sMessageDeviceNode.toLatin1().data(), O_RDWR)) < 0 )
    {
        if (m_pDebugSettings->getDebugLevel() & 1)  syslog(LOG_ERR,"error opening ctrl device: %s\n",m_pFPGASettings->getDeviceNode().toLatin1().data());
    }
    return DevFileDescriptorMsg;
}


void cMT310S2dServer::SetFASync()
{
    fcntl(DevFileDescriptorMsg, F_SETOWN, getpid()); // wir sind "besitzer" des device
    int oflags = fcntl(DevFileDescriptorMsg, F_GETFL);
    fcntl(DevFileDescriptorMsg, F_SETFL, oflags | FASYNC); // async. benachrichtung (sigio) einschalten

    fcntl(DevFileDescriptorCtrl, F_SETOWN, getpid());
    oflags = fcntl(DevFileDescriptorCtrl, F_GETFL);
    fcntl(DevFileDescriptorCtrl, F_SETFL, oflags | FASYNC);
}


void cMT310S2dServer::enableClampInterrupt()
{
    quint16 maskToAdd = (1 << clampstatusInterrupt);
    if(pAtmel->writeIntMask(m_atmelInterruptMask | maskToAdd) == ZeraMcontrollerBase::cmddone) {
        m_atmelInterruptMask |= maskToAdd;
    }
    else {
        qWarning("Could not enable clamp interrupt!");
    }
}

void cMT310S2dServer::updateI2cDevicesConnected()
{
    quint16 clStat;
    if ( pAtmel->readClampStatus(clStat) == ZeraMcontrollerBase::cmddone) {
        qInfo("Devices connected mask read: 0x%02X", clStat);
        m_pClampInterface->actualizeClampStatus(clStat);
    }
    else {
        qWarning("Devices connected mask read failed");
    }
}


void cMT310S2dServer::MTIntHandler(int)
{// handles clamp interrupt sent by the controler

    char buf[2];
    quint16 stat = 0;

    read(pipeFD[0], buf, 1); // first we read the pipe

    if ( pAtmel->readCriticalStatus(stat) == ZeraMcontrollerBase::cmddone ) {
        if ((stat & (1 << clampstatusInterrupt)) > 0) {
            // we must reset clamp status before handling the interrupt
            // because system may emit more interrupts so we might think
            // we must handle them too....but atualizeClampStatus uses
            // global variable for the clamp status -> so this might fail
            // with unexpected behaviour.
            pAtmel->resetCriticalStatus(stat & (1 << clampstatusInterrupt));
            updateI2cDevicesConnected();
        }
        quint16 knownMaskBits = (1 << clampstatusInterrupt);
        if((stat & ~knownMaskBits) > 0) {
            qWarning("cMT310S2dServer::MTIntHandler: unknown bits in critical status mask: %04x!", stat);
        }
    }
    else {
        qWarning("cMT310S2dServer::MTIntHandler: pAtmel->readCriticalStatus failed - cannot actualize clamp status!");
    }

    // here we must add the handling for message interrupts sent by fpga device
    /*if (false)
    {
        QString message;
        message = QString("Error");
        qFatal(message.toLatin1());
        qWarning(message.toLatin1());
    }*/
}


