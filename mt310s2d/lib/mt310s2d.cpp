#include "mt310s2d.h"
#include "hotpluggablecontrollercontainer.h"
#include "mt310s2dglobal.h"
#include "mt310s2systeminterface.h"
#include "mt310s2senseinterface.h"
#include "clampinterface.h"
#include "fingroupresourceandinterface.h"
#include "hkingroupresourceandinterface.h"
#include "samplinginterface.h"
#include "scingroupresourceandinterface.h"
#include "foutgroupresourceandinterface.h"
#include "servicestatusinterface.h"
#include <QSocketNotifier>
#include <QCoreApplication>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

static int pipeFileDescriptorMt310s2[2];
static void SigHandler(int)
{
    const static char pipeFDBuf[2] = "I";
    write(pipeFileDescriptorMt310s2[1], pipeFDBuf, 1);
}

static struct sigaction sigActionMt310s2;
// sigset_t mySigmask, origSigmask;


cMT310S2dServer::cMT310S2dServer(SettingsContainerPtr settings,
                                 AbstractFactoryI2cCtrlPtr ctrlFactory,
                                 AbstractFactoryDeviceNodePcbPtr deviceNodeFactory,
                                 VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                 AbstractChannelRangeFactoryPtr channelRangeFactory,
                                 AbstractHotPluggableControllerContainerPtr hotplugContainer) :
    PCBServer(std::move(settings), tcpNetworkFactory),
    m_ctrlFactory(ctrlFactory),
    m_deviceNodeFactory(deviceNodeFactory),
    m_i2cCtrlCpuTemperature(ctrlFactory->getCpuTemperatureController())
{
    doConfiguration();
    init();
    earlySetup(channelRangeFactory, hotplugContainer);
}

void cMT310S2dServer::init()
{
    m_pInitializationMachine = new QStateMachine(this);

    QState* stateCONF = new QState(); // we start from here
    QFinalState* stateFINISH = new QFinalState(); // and here we finish

    stateCONF->addTransition(this, &cMT310S2dServer::abortInit, stateFINISH); // from anywhere we arrive here if some error

    QState* statewait4Atmel = new QState(stateCONF); // we synchronize on atmel running
    QState* statesetupServer = new QState(stateCONF); // we setup our server now
    m_stateconnect2RM = new QState(stateCONF); // we connect to resource manager
    m_stateconnect2RMError = new QState(stateCONF);
    m_stateSendRMIdentAndRegister = new QState(stateCONF); // we send ident. to rm and register our resources

    stateCONF->setInitialState(statewait4Atmel);

    statewait4Atmel->addTransition(this, &cMT310S2dServer::atmelRunning, statesetupServer);
    statesetupServer->addTransition(this, &cMT310S2dServer::sigServerIsSetUp, m_stateconnect2RM);

    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);

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
    delete m_pSenseSettings;
    delete m_foutSettings;
    delete m_finSettings;
    delete m_pSCHeadSettings;
    delete m_accumulatorSettings;
    delete m_sourceControlSettings;
    delete m_pStatusInterface;
    delete m_pSystemInterface;
    delete m_pSenseInterface;
    delete m_pClampInterface;
    delete m_foutInterface;
    delete m_pFRQInputInterface;
    delete m_pSCHeadInterface;
    delete m_hkInInterface;
    delete m_pSystemInfo;
    delete m_pRMConnection;
    delete m_accumulatorInterface;
}

QString cMT310S2dServer::getCtrlDeviceNode()
{
    return m_settings->getFpgaSettings()->getCtrlDeviceNode();
}

QString cMT310S2dServer::getMsgDeviceNode()
{
    return m_settings->getFpgaSettings()->getMsgDeviceNode();
}

void cMT310S2dServer::setupMicroControllerIo()
{
    PermissionFunctions::setPermissionCtrlFactory(m_ctrlFactory);
    m_ctrlHeartbeatWait = m_ctrlFactory->createCtrlHeartbeatWait(getCtrlDeviceNode());
}

void cMT310S2dServer::doConfiguration()
{
    if ( pipe(pipeFileDescriptorMt310s2) == -1) {
        qCritical("Abort, could not open pipe");
        emit abortInit();
    }
    else {
        fcntl( pipeFileDescriptorMt310s2[1], F_SETFL, O_NONBLOCK);
        fcntl( pipeFileDescriptorMt310s2[0], F_SETFL, O_NONBLOCK);
        m_pNotifier = new QSocketNotifier(pipeFileDescriptorMt310s2[0], QSocketNotifier::Read, this);
        connect(m_pNotifier, &QSocketNotifier::activated, this, &cMT310S2dServer::MTIntHandler);

        ServerParams params = m_settings->getServerParams();
        m_pSenseSettings = new cSenseSettings(&m_xmlConfigReader, 8);
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_pSenseSettings, &cSenseSettings::configXMLInfo);
        m_foutSettings = new FOutSettings(&m_xmlConfigReader);
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_foutSettings, &FOutSettings::configXMLInfo);
        m_finSettings = new FInSettings(&m_xmlConfigReader);
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_finSettings, &FInSettings::configXMLInfo);
        m_pSCHeadSettings = new ScInSettings(&m_xmlConfigReader);
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_pSCHeadSettings, &ScInSettings::configXMLInfo);
        m_hkInSettings = new HkInSettings(&m_xmlConfigReader);
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_hkInSettings, &HkInSettings::configXMLInfo);
        m_accumulatorSettings = new AccumulatorSettings(&m_xmlConfigReader);
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_accumulatorSettings, &AccumulatorSettings::configXMLInfo);
        m_sourceControlSettings = new SourceControlSettings(&m_xmlConfigReader);
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_sourceControlSettings, &SourceControlSettings::configXMLInfo);

        if (m_xmlConfigReader.loadXMLFile(params.xmlFile))
            setupMicroControllerIo();
        else {
            qCritical("Abort: Could not open xml file '%s", qPrintable(params.xmlFile));
            emit abortInit();
        }
    }
}


void cMT310S2dServer::doWait4Atmel()
{
    connect(m_ctrlHeartbeatWait.get(), &AbstractCtrlHeartbeatWait::sigTimeout,
            this, &cMT310S2dServer::abortInit);
    connect(m_ctrlHeartbeatWait.get(), &AbstractCtrlHeartbeatWait::sigRunning,
            this, &cMT310S2dServer::atmelRunning);
    m_ctrlHeartbeatWait->start();
}


void cMT310S2dServer::earlySetup(AbstractChannelRangeFactoryPtr channelRangeFactory,
                                 AbstractHotPluggableControllerContainerPtr hotplugContainer)
{
    qInfo("Set initial PLL channel...");
    m_ctrlFactory->getPllController()->setPLLChannel(1); // default channel m0 for pll control
    qInfo("Initial PLL channel set");
    m_pSystemInfo = new Mt310s2SystemInfo(m_ctrlFactory);

    connectProtoConnectionSignals();

    m_scpiConnectionList.append(this); // the server itself has some commands
    I2cSettings *i2cSettings = m_settings->getI2cSettings();
    m_scpiConnectionList.append(m_pSenseInterface = new Mt310s2SenseInterface(m_scpiInterface,
                                                                              i2cSettings,
                                                                              m_pSenseSettings,
                                                                              m_pSystemInfo,
                                                                              channelRangeFactory,
                                                                              m_ctrlFactory));
    m_scpiConnectionList.append(m_pStatusInterface = new ServiceStatusInterface(m_scpiInterface, m_pSenseInterface, m_ctrlFactory));
    m_scpiConnectionList.append(m_pSystemInterface = new Mt310s2SystemInterface(this,
                                                                                m_pSystemInfo,
                                                                                m_pSenseSettings,
                                                                                m_pSenseInterface,
                                                                                m_ctrlFactory,
                                                                                hotplugContainer));
    m_scpiConnectionList.append(m_pSamplingInterface = new cSamplingInterface(m_scpiInterface, m_settings->getSamplingSettings(), m_ctrlFactory));
    m_scpiConnectionList.append(m_foutInterface = new FOutGroupResourceAndInterface(m_scpiInterface, m_foutSettings));
    m_scpiConnectionList.append(m_pFRQInputInterface = new FInGroupResourceAndInterface(m_scpiInterface, m_finSettings));
    m_scpiConnectionList.append(m_pSCHeadInterface = new ScInGroupResourceAndInterface(m_scpiInterface, m_pSCHeadSettings));
    m_scpiConnectionList.append(m_hkInInterface = new HkInGroupResourceAndInterface(m_scpiInterface, m_hkInSettings));
    m_scpiConnectionList.append(m_pClampInterface = new cClampInterface(this,
                                                                        i2cSettings,
                                                                        m_pSenseSettings,
                                                                        m_pSenseInterface,
                                                                        m_ctrlFactory));
    m_scpiConnectionList.append(m_accumulatorInterface = new AccumulatorInterface(m_scpiInterface, m_accumulatorSettings, m_ctrlFactory));
    connect(m_accumulatorInterface, &AccumulatorInterface::sigAccumulatorStatusChange,
            m_pSystemInterface, &Mt310s2SystemInterface::onAccuStatusChanged);
    m_scpiConnectionList.append(m_sourceControlInterface = new SourceControlInterface(m_scpiInterface, m_sourceControlSettings, m_ctrlFactory));

    m_resourceList.append(m_pSenseInterface); // all our resources
    m_resourceList.append(m_pSamplingInterface);
    m_resourceList.append(m_foutInterface);
    m_resourceList.append(m_pFRQInputInterface);
    m_resourceList.append(m_pSCHeadInterface);
    m_resourceList.append(m_hkInInterface);
    qInfo("SCPI interfaces set.");
}

void cMT310S2dServer::doSetupServer()
{
    qInfo("Starting doSetupServer");
    QString ctrlDeviceNodeName = getCtrlDeviceNode(); // we try to open the ctrl device
    AbstractDeviceNodePcbCtrlPtr ctrlDeviceNode = m_deviceNodeFactory->getPcbCtrlDeviceNode();
    if (ctrlDeviceNode->open(ctrlDeviceNodeName) < 0) {
        qCritical("Abort: Could not open control device '%s'", qPrintable(ctrlDeviceNodeName));
        emit abortInit();
    }
    else {
        QString messageDeviceNodeName = getMsgDeviceNode();
        AbstractDeviceNodePcbMsgPtr msgDeviceNode = m_deviceNodeFactory->getPcbMsgDeviceNode();
        if (msgDeviceNode->open(messageDeviceNodeName) < 0) {
            qCritical("Abort: Could not open message device '%s'", qPrintable(messageDeviceNodeName));
            emit abortInit();
        }
        else {
            initSCPIConnections();

            // after init. we once poll the devices connected at power up
            updateI2cDevicesConnected();

            sigActionMt310s2.sa_handler = &SigHandler; // setup signal handler
            sigemptyset(&sigActionMt310s2.sa_mask);
            sigActionMt310s2. sa_flags = SA_RESTART;
            sigActionMt310s2.sa_restorer = nullptr;
            sigaction(SIGIO, &sigActionMt310s2, nullptr); // set handler for sigio

            SetFASync();
            enableClampInterrupt();

            // our resource mananager connection must be opened after configuration is done
            EthSettings *ethSettings = m_settings->getEthSettings();
            m_pRMConnection = new RMConnection(ethSettings->getRMIPadr(),
                                               ethSettings->getPort(EthSettings::resourcemanager),
                                               m_tcpNetworkFactory);
            // so we must complete our state machine here
            m_retryRMConnect = 100;
            m_retryTimer.setSingleShot(true);
            connect(&m_retryTimer, &QTimer::timeout, this, &cMT310S2dServer::sigServerIsSetUp);

            m_stateconnect2RM->addTransition(m_pRMConnection, &RMConnection::connected, m_stateSendRMIdentAndRegister);
            m_stateconnect2RM->addTransition(m_pRMConnection, &RMConnection::connectionRMError, m_stateconnect2RMError);
            m_stateconnect2RMError->addTransition(this, &cMT310S2dServer::sigServerIsSetUp, m_stateconnect2RM);

            emit sigServerIsSetUp(); // so we enter state machine's next state

            startCpuTemperatureSendTimer();
        }
    }
}


void cMT310S2dServer::doCloseServer()
{
    QCoreApplication::instance()->exit(-1);
}


void cMT310S2dServer::doConnect2RM()
{
    qInfo("Starting doConnect2RM");
    m_pRMConnection->connect2RM();
}


void cMT310S2dServer::connect2RMError()
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


void cMT310S2dServer::doIdentAndRegister()
{
    qInfo("Starting doIdentAndRegister");
    m_pRMConnection->SendIdent(getName());
    for (int i = 0; i < m_resourceList.count(); i++) {
        cResource *res = m_resourceList.at(i);
        connect(m_pRMConnection, &RMConnection::rmAck, res, &cResource::resourceManagerAck );
        EthSettings *ethSettings = m_settings->getEthSettings();
        res->registerResource(m_pRMConnection, ethSettings->getPort(EthSettings::protobufserver));
        connect(res, &cResource::registerRdy, this, &cMT310S2dServer::onResourceReady);
    }
    m_pendingResources = m_resourceList.count();
}

void cMT310S2dServer::onResourceReady()
{
    Q_ASSERT(m_pendingResources > 0);
    m_pendingResources--;
    disconnect(static_cast<cResource*>(sender()), &cResource::registerRdy, this, &cMT310S2dServer::onResourceReady);
    if(m_pendingResources == 0) {
        EthSettings *ethSettings = m_settings->getEthSettings();
        m_protoBufServer.startServer(ethSettings->getPort(EthSettings::protobufserver));
        openTelnetScpi();
    }
}


void cMT310S2dServer::SetFASync()
{
    AbstractDeviceNodePcbMsgPtr msgDeviceNode = m_deviceNodeFactory->getPcbMsgDeviceNode();
    msgDeviceNode->enableFasync();
    AbstractDeviceNodePcbCtrlPtr ctrlDeviceNode = m_deviceNodeFactory->getPcbCtrlDeviceNode();
    ctrlDeviceNode->enableFasync();
}


void cMT310S2dServer::enableClampInterrupt()
{
    quint16 maskToAdd = (1 << clampstatusInterrupt);
    if(m_ctrlFactory->getCriticalStatusController()->writeIntMask(m_atmelInterruptMask | maskToAdd) == ZeraMControllerIo::cmddone) {
        m_atmelInterruptMask |= maskToAdd;
    }
    else {
        qWarning("Could not enable clamp interrupt!");
    }
}

void cMT310S2dServer::updateI2cDevicesConnected()
{
    quint16 clStat;
    qInfo("Read device connect mask...");
    if (m_ctrlFactory->getClampStatusController()->readClampStatus(clStat) == ZeraMControllerIo::cmddone) {
        qInfo("Devices connected mask read: 0x%02X", clStat);
        m_pClampInterface->actualizeClampStatus(clStat);
        m_pSystemInterface->actualizeControllers(clStat);
    }
    else
        qWarning("Devices connected mask read failed");
}


void cMT310S2dServer::MTIntHandler(int)
{// handles clamp interrupt sent by the controler

    char dummy[2];
    read(pipeFileDescriptorMt310s2[0], dummy, 1); // first we read the pipe

    quint16 stat = 0;
    if (m_ctrlFactory->getCriticalStatusController()->readCriticalStatus(stat) == ZeraMControllerIo::cmddone ) {
        if ((stat & (1 << clampstatusInterrupt)) > 0) {
            // we must reset clamp status before handling the interrupt
            // because system may emit more interrupts so we might think
            // we must handle them too....but atualizeClampStatus uses
            // global variable for the clamp status -> so this might fail
            // with unexpected behaviour.
            m_ctrlFactory->getCriticalStatusController()->resetCriticalStatus(stat & (1 << clampstatusInterrupt));
            updateI2cDevicesConnected();
        }
        quint16 knownMaskBits = (1 << clampstatusInterrupt);
        if((stat & ~knownMaskBits) > 0) {
            qWarning("cMT310S2dServer::MTIntHandler: unknown bits in critical status mask: %04x!", stat);
        }
    }
    else
        qWarning("cMT310S2dServer::MTIntHandler: readCriticalStatus failed - cannot actualize clamp status!");
}

void cMT310S2dServer::startCpuTemperatureSendTimer()
{
    if (m_cpuTemperature.canGetValue()) {
        qInfo("Initialize CPU-temperature timer for system-controller temperature notification");
        m_1sPeriodicTimer = TimerFactoryQt::createPeriodic(1000);
        connect(m_1sPeriodicTimer.get(), &TimerTemplateQt::sigExpired,
                this, &cMT310S2dServer::onCpuTemperatureSend);
        m_1sPeriodicTimer->start();
    }
    else
        qWarning("CPU-temperature is not available on this device. Temperature is not send to system-controller!");
}

void cMT310S2dServer::onCpuTemperatureSend()
{
    float temperature = m_cpuTemperature.getValue();
    if (temperature > 0.0)
        m_i2cCtrlCpuTemperature->sendCpuTemperature(temperature);
    else
        qWarning("CPU-temperature read is less than 0°!");
}
