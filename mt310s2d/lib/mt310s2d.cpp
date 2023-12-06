#include "mt310s2d.h"
#include "mt310s2dglobal.h"
#include "adjustment.h"
#include "rmconnection.h"
#include "atmelsysctrl.h"
#include "atmel.h"
#include "atmelctrlfactory.h"
#include "clampinterface.h"
#include "fingroupresourceandinterface.h"
#include "hkingroupresourceandinterface.h"
#include "samplinginterface.h"
#include "scingroupresourceandinterface.h"
#include "senseinterface.h"
#include "foutgroupresourceandinterface.h"
#include "statusinterface.h"
#include "systeminterface.h"
#include "ctrlsettings.h"
#include "debugsettings.h"
#include "ethsettings.h"
#include "finsettings.h"
#include "fpgasettings.h"
#include "hkinsettings.h"
#include "i2csettings.h"
#include "samplingsettings.h"
#include "scinsettings.h"
#include "sensesettings.h"
#include "foutsettings.h"
#include "pcbdevicenodectrlsingleton.h"
#include "pcbdevicenodemessagesingleton.h"
#include <scpisingletonfactory.h>
#include <xmlconfigreader.h>
#include <xiqnetserver.h>
#include <QSocketNotifier>
#include <QCoreApplication>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <signal.h>

#ifdef SYSTEMD_NOTIFICATION
#include <systemd/sd-daemon.h>
#endif

static int pipeFD[2];
void SigHandler(int)
{
    const static char pipeFDBuf[2] = "I";
    write(pipeFD[1], pipeFDBuf, 1);
}

static struct sigaction mySigAction;
// sigset_t mySigmask, origSigmask;


cATMELSysCtrl* pAtmelSys;
ServerParams cMT310S2dServer::defaultParams {ServerName, ServerVersion, "/etc/zera/mt310s2d/mt310s2d.xsd", "/etc/zera/mt310s2d/mt310s2d.xml"};

cMT310S2dServer::cMT310S2dServer(ServerParams params) :
    cPCBServer(params, ScpiSingletonFactory::getScpiObj()),
    m_params(params)
{
    m_pDebugSettings = nullptr;
    m_pI2CSettings = nullptr;
    m_pFPGASettings = nullptr;
    m_pCtrlSettings  = nullptr;
    m_pSenseSettings = nullptr;
    pAtmelSys = nullptr;
    m_pStatusInterface = nullptr;
    m_pSystemInterface = nullptr;
    m_pSenseInterface = nullptr;
    m_pClampInterface = nullptr;
    m_pSystemInfo = nullptr;
    m_pAdjHandler = nullptr;
    m_pRMConnection = nullptr;
    m_accumulatorInterface = nullptr;

    m_pInitializationMachine = new QStateMachine(this);

    QState* stateCONF = new QState(); // we start from here
    QFinalState* stateFINISH = new QFinalState(); // and here we finish

    stateCONF->addTransition(this, SIGNAL(abortInit()),stateFINISH); // from anywhere we arrive here if some error

    QState* statexmlConfiguration = new QState(stateCONF); // we configure our server with xml file
    QState* statewait4Atmel = new QState(stateCONF); // we synchronize on atmel running
    QState* statesetupServer = new QState(stateCONF); // we setup our server now
    m_stateconnect2RM = new QState(stateCONF); // we connect to resource manager
    m_stateconnect2RMError = new QState(stateCONF);
    m_stateSendRMIdentAndRegister = new QState(stateCONF); // we send ident. to rm and register our resources

    stateCONF->setInitialState(statexmlConfiguration);

    statexmlConfiguration->addTransition(&m_xmlConfigReader, SIGNAL(finishedParsingXML(bool)), statewait4Atmel);
    statewait4Atmel->addTransition(this, SIGNAL(atmelRunning()), statesetupServer);
    statesetupServer->addTransition(this, SIGNAL(sigServerIsSetUp()), m_stateconnect2RM);

    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);

    QObject::connect(statexmlConfiguration, &QAbstractState::entered, this, &cMT310S2dServer::doConfiguration);
    QObject::connect(statewait4Atmel, &QAbstractState::entered, this, &cMT310S2dServer::doWait4Atmel);
    QObject::connect(statesetupServer, &QAbstractState::entered, this, &cMT310S2dServer::doSetupServer);
    QObject::connect(m_stateconnect2RM, &QAbstractState::entered, this, &cMT310S2dServer::doConnect2RM);
    QObject::connect(m_stateconnect2RMError, &QAbstractState::entered, this, &cMT310S2dServer::connect2RMError);
    QObject::connect(m_stateSendRMIdentAndRegister, &QAbstractState::entered, this, &cMT310S2dServer::doIdentAndRegister);
    QObject::connect(stateFINISH, &QAbstractState::entered, this, &cMT310S2dServer::doCloseServer);

    m_pInitializationMachine->start();
}


cMT310S2dServer::~cMT310S2dServer()
{
    if (m_pDebugSettings) delete m_pDebugSettings;
    if (m_pI2CSettings) delete m_pI2CSettings;
    if (m_pFPGASettings) delete m_pFPGASettings;
    if (m_pCtrlSettings) delete m_pCtrlSettings;
    if (m_pSenseSettings) delete m_pSenseSettings;
    if (m_foutSettings) delete m_foutSettings;
    if (m_finSettings) delete m_finSettings;
    if (m_pSCHeadSettings) delete m_pSCHeadSettings;
    if (m_accumulatorSettings) delete m_accumulatorSettings;
    if (pAtmelSys) delete pAtmelSys;
    if (m_pStatusInterface) delete m_pStatusInterface;
    if (m_pSystemInterface) delete m_pSystemInterface;
    if (m_pSenseInterface) delete m_pSenseInterface;
    if (m_pClampInterface) delete m_pClampInterface;
    if (m_foutInterface) delete m_foutInterface;
    if (m_pFRQInputInterface) delete m_pFRQInputInterface;
    if (m_pSCHeadInterface) delete m_pSCHeadInterface;
    if (m_hkInInterface) delete m_hkInInterface;
    if (m_pSystemInfo) delete m_pSystemInfo;
    if (m_pAdjHandler) delete m_pAdjHandler;
    if (m_pRMConnection) delete m_pRMConnection;
    if (m_accumulatorInterface) delete m_accumulatorInterface;
}

void cMT310S2dServer::doConfiguration()
{
    if ( pipe(pipeFD) == -1) {
        qCritical("Abort, could not open pipe");
        emit abortInit();
    }
    else {
        fcntl( pipeFD[1], F_SETFL, O_NONBLOCK);
        fcntl( pipeFD[0], F_SETFL, O_NONBLOCK);
        m_pNotifier = new QSocketNotifier(pipeFD[0], QSocketNotifier::Read, this);
        connect(m_pNotifier, &QSocketNotifier::activated, this, &cMT310S2dServer::MTIntHandler);

        if (m_xmlConfigReader.loadSchema(m_params.xsdFile)) {
            // we want to initialize all settings first
            m_pDebugSettings = new cDebugSettings(&m_xmlConfigReader);
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_pDebugSettings, &cDebugSettings::configXMLInfo);
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, &m_ethSettings, &EthSettings::configXMLInfo);
            m_pI2CSettings = new cI2CSettings(&m_xmlConfigReader);
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_pI2CSettings, &cI2CSettings::configXMLInfo);
            m_pFPGASettings = new FPGASettings(&m_xmlConfigReader);
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_pFPGASettings, &FPGASettings::configXMLInfo);
            m_pCtrlSettings = new cCtrlSettings(&m_xmlConfigReader);
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_pCtrlSettings, &cCtrlSettings::configXMLInfo);
            m_pSenseSettings = new cSenseSettings(&m_xmlConfigReader, 8);
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_pSenseSettings, &cSenseSettings::configXMLInfo);
            m_foutSettings = new FOutSettings(&m_xmlConfigReader);
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_foutSettings, &FOutSettings::configXMLInfo);
            m_pSamplingSettings = new SamplingSettings(&m_xmlConfigReader);
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_pSamplingSettings, &SamplingSettings::configXMLInfo);
            m_finSettings = new FInSettings(&m_xmlConfigReader);
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_finSettings, &FInSettings::configXMLInfo);
            m_pSCHeadSettings = new ScInSettings(&m_xmlConfigReader);
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_pSCHeadSettings, &ScInSettings::configXMLInfo);
            m_hkInSettings = new HkInSettings(&m_xmlConfigReader);
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_hkInSettings, &HkInSettings::configXMLInfo);
            m_accumulatorSettings = new AccumulatorSettings(&m_xmlConfigReader);
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_accumulatorSettings, &AccumulatorSettings::configXMLInfo);

            if (!m_xmlConfigReader.loadXMLFile(m_params.xmlFile)) {
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


void cMT310S2dServer::doWait4Atmel()
{
    // a singletom for atmel would be nice...
    pAtmelSys = new cATMELSysCtrl(m_pI2CSettings->getDeviceNode(), m_pI2CSettings->getI2CAdress(i2cSettings::sysCtrlI2cAddress), m_pDebugSettings->getDebugLevel());
    Atmel::setInstanceParams(m_pI2CSettings->getDeviceNode(), m_pI2CSettings->getI2CAdress(i2cSettings::relaisCtrlI2cAddress), m_pDebugSettings->getDebugLevel());
    m_atmelWatcher = AtmelCtrlFactory::createAtmelWatcher(m_pCtrlSettings->getDeviceNode());

    m_nerror = atmelError; // we preset error
    connect(m_atmelWatcher.get(), &AtmelWatcherInterface::sigTimeout,
            this, &cMT310S2dServer::abortInit);
    connect(m_atmelWatcher.get(), &AtmelWatcherInterface::sigRunning,
            this, &cMT310S2dServer::atmelRunning);
    m_atmelWatcher->start();
}


void cMT310S2dServer::doSetupServer()
{
    QString ctrlDeviceNodeName = m_pCtrlSettings->getDeviceNode(); // we try to open the ctrl device
    if (PcbDeviceNodeCtrlSingleton::getInstance()->open(ctrlDeviceNodeName) < 0) {
        qCritical("Abort: Could not open control device '%s'", qPrintable(ctrlDeviceNodeName));
        emit abortInit();
    }
    else {
        QString messageDeviceNodeName = m_pFPGASettings->getDeviceNode();
        if (PcbDeviceNodeMessageSingleton::getInstance()->open(messageDeviceNodeName) < 0) {
            qCritical("Abort: Could not open message device '%s'", qPrintable(messageDeviceNodeName));
            emit abortInit();
        }
        else
        {
            Atmel::getInstance().setPLLChannel(1); // default channel m0 for pll control
            m_pSystemInfo = new Mt310s2SystemInfo();
            m_pAdjHandler = new cAdjustment(this);

            setupServer(); // here our scpi interface gets instanciated, we need this for further steps

            scpiConnectionList.append(this); // the server itself has some commands
            scpiConnectionList.append(m_pStatusInterface = new cStatusInterface(getSCPIInterface(), m_pAdjHandler));
            HotPluggableControllerContainerPtr emobControllerContainer =
                    std::make_unique<HotPluggableControllerContainer>(m_pI2CSettings->getDeviceNode(),
                                                                      m_pI2CSettings->getI2CAdress(i2cSettings::emobCtrlI2cAddress),
                                                                      m_pI2CSettings->getI2CAdress(i2cSettings::muxerI2cAddress),
                                                                      m_pDebugSettings->getDebugLevel());
            scpiConnectionList.append(m_pSystemInterface = new cSystemInterface(this, std::move(emobControllerContainer)));
            scpiConnectionList.append(m_pSenseInterface = new cSenseInterface(this));
            scpiConnectionList.append(m_pSamplingInterface = new cSamplingInterface(getSCPIInterface(), m_pSamplingSettings));
            scpiConnectionList.append(m_foutInterface = new FOutGroupResourceAndInterface(getSCPIInterface(), m_foutSettings));
            scpiConnectionList.append(m_pFRQInputInterface = new FInGroupResourceAndInterface(getSCPIInterface(), m_finSettings));
            scpiConnectionList.append(m_pSCHeadInterface = new ScInGroupResourceAndInterface(getSCPIInterface(), m_pSCHeadSettings));
            scpiConnectionList.append(m_hkInInterface = new HkInGroupResourceAndInterface(getSCPIInterface(), m_hkInSettings));
            scpiConnectionList.append(m_pClampInterface = new cClampInterface(this));
            scpiConnectionList.append(m_accumulatorInterface = new AccumulatorInterface(getSCPIInterface(), pAtmelSys, m_accumulatorSettings));

            resourceList.append(m_pSenseInterface); // all our resources
            resourceList.append(m_pSamplingInterface);
            resourceList.append(m_foutInterface);
            resourceList.append(m_pFRQInputInterface);
            resourceList.append(m_pSCHeadInterface);
            resourceList.append(m_hkInInterface);


            m_pAdjHandler->addAdjFlashObject(m_pSenseInterface);
            m_pSenseInterface->importAdjFlash(); // we read adjustmentdata at least once

            initSCPIConnections();

            // after init. we once poll the devices connected at power up
            updateI2cDevicesConnected();

            m_myServer->startServer(m_ethSettings.getPort(EthSettings::protobufserver)); // and can start the server now
            if(m_ethSettings.isSCPIactive())
                m_pSCPIServer->listen(QHostAddress::AnyIPv4, m_ethSettings.getPort(EthSettings::scpiserver));

            mySigAction.sa_handler = &SigHandler; // setup signal handler
            sigemptyset(&mySigAction.sa_mask);
            mySigAction. sa_flags = SA_RESTART;
            mySigAction.sa_restorer = nullptr;
            sigaction(SIGIO, &mySigAction, nullptr); // set handler for sigio

            SetFASync();
            enableClampInterrupt();

            // our resource mananager connection must be opened after configuration is done
            m_pRMConnection = new RMConnection(m_ethSettings.getRMIPadr(), m_ethSettings.getPort(EthSettings::resourcemanager));
            //connect(m_pRMConnection, SIGNAL(connectionRMError()), this, SIGNAL(abortInit()));
            // so we must complete our state machine here
            m_retryRMConnect = 100;
            m_retryTimer.setSingleShot(true);
            connect(&m_retryTimer, &QTimer::timeout, this, &cMT310S2dServer::sigServerIsSetUp);

            m_stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connected()), m_stateSendRMIdentAndRegister);
            m_stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connectionRMError()), m_stateconnect2RMError);
            m_stateconnect2RMError->addTransition(this, SIGNAL(sigServerIsSetUp()), m_stateconnect2RM);

            emit sigServerIsSetUp(); // so we enter state machine's next state
        }
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
    m_retryRMConnect--;
    if (m_retryRMConnect == 0)
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
        res->registerResource(m_pRMConnection, m_ethSettings.getPort(EthSettings::protobufserver));
    }
#ifdef SYSTEMD_NOTIFICATION
    sd_notify(0, "READY=1");
#endif
}


void cMT310S2dServer::SetFASync()
{
    PcbDeviceNodeMessageSingleton::getInstance()->enableFasync();
    PcbDeviceNodeCtrlSingleton::getInstance()->enableFasync();
}


void cMT310S2dServer::enableClampInterrupt()
{
    quint16 maskToAdd = (1 << clampstatusInterrupt);
    if(Atmel::getInstance().writeIntMask(m_atmelInterruptMask | maskToAdd) == ZeraMControllerIo::cmddone) {
        m_atmelInterruptMask |= maskToAdd;
    }
    else {
        qWarning("Could not enable clamp interrupt!");
    }
}

void cMT310S2dServer::enableAccumulatorInterrupt()
{
    quint16 maskToAdd = (1 << accumulatorInterrupt);
    if(pAtmelSys->writeIntMask(m_atmelSysCntrlInterruptMask | maskToAdd) == ZeraMControllerIo::cmddone) {
        m_atmelSysCntrlInterruptMask |= maskToAdd;
    }
    else {
        qWarning("Could not enable accumulator interrupt!");
    }
}

void cMT310S2dServer::updateI2cDevicesConnected()
{
    quint16 clStat;
    if ( Atmel::getInstance().readClampStatus(clStat) == ZeraMControllerIo::cmddone) {
        qInfo("Devices connected mask read: 0x%02X", clStat);
        m_pClampInterface->actualizeClampStatus(clStat);
        m_pSystemInterface->actualizeContollers(clStat);
    }
    else {
        qWarning("Devices connected mask read failed");
    }
}


void cMT310S2dServer::MTIntHandler(int)
{// handles clamp interrupt sent by the controler

    char dummy[2];
    read(pipeFD[0], dummy, 1); // first we read the pipe

    quint16 stat = 0;
    if ( Atmel::getInstance().readCriticalStatus(stat) == ZeraMControllerIo::cmddone ) {
        if ((stat & (1 << clampstatusInterrupt)) > 0) {
            // we must reset clamp status before handling the interrupt
            // because system may emit more interrupts so we might think
            // we must handle them too....but atualizeClampStatus uses
            // global variable for the clamp status -> so this might fail
            // with unexpected behaviour.
            Atmel::getInstance().resetCriticalStatus(stat & (1 << clampstatusInterrupt));
            updateI2cDevicesConnected();
        }
        quint16 knownMaskBits = (1 << clampstatusInterrupt);
        if((stat & ~knownMaskBits) > 0) {
            qWarning("cMT310S2dServer::MTIntHandler: unknown bits in critical status mask: %04x!", stat);
        }
    }
    else
        qWarning("cMT310S2dServer::MTIntHandler: Atmel::getInstance().readCriticalStatus failed - cannot actualize clamp status!");
}


