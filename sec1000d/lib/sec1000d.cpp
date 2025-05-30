#include "sec1000d.h"
#include "ethsettings.h"
#include "seccalcsettings.h"
#include "secinputsettings.h"
#include "sec1000statusinterface.h"
#include "sec1000systeminterface.h"
#include "sec1000systeminfo.h"
#include "secgroupresourceandinterface.h"
#include "rmconnection.h"
#include <vtcp_server.h>
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

#define ServerName "sec1000d"
#define ServerVersion "V1.00"

const int cSEC1000dServer::Com5003EcUnitCount = 8;
const int cSEC1000dServer::Mtxxxs2EcUnitCount = 15;

cSEC1000dServer* SECServer;

int pipeFileDescriptorSec1000[2];
static void SigHandler(int)
{
    const static char pipeFDBuf[2] = "I";
    write(pipeFileDescriptorSec1000[1], pipeFDBuf, 1);
}

struct sigaction sigActionSec1000;

const ServerParams cSEC1000dServer::defaultParams{ServerName, ServerVersion, "/etc/zera/sec1000d/sec1000d.xsd", "/etc/zera/sec1000d/sec1000d.xml"};

cSEC1000dServer::cSEC1000dServer(SettingsContainerPtr settings,
                                 int ecUnitCount,
                                 AbstractFactoryDeviceNodeSecPtr deviceNodeFactory,
                                 VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory) :
    PCBServer(std::move(settings), tcpNetworkFactory),
    m_deviceNodeFactory(deviceNodeFactory)
{
    doConfiguration(ecUnitCount);
    init();
}

void cSEC1000dServer::init()
{
    m_pInitializationMachine = new QStateMachine(this);

    QState* stateCONF = new QState(); // we start from here
    QFinalState* stateFINISH = new QFinalState(); // and here we finish

    stateCONF->addTransition(this, &cSEC1000dServer::abortInit, stateFINISH); // from anywhere we arrive here if some error

    QState* statesetupServer = new QState(stateCONF); // we setup our server now
    m_stateconnect2RM = new QState(stateCONF); // we connect to resource manager
    m_stateconnect2RMError = new QState(stateCONF);
    m_stateSendRMIdentAndRegister = new QState(stateCONF); // we send ident. to rm and register our resources
    stateCONF->setInitialState(statesetupServer);

    statesetupServer->addTransition(this, &cSEC1000dServer::sigServerIsSetUp, m_stateconnect2RM);
    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);

    QObject::connect(statesetupServer, &QAbstractState::entered, this, &cSEC1000dServer::doSetupServer);
    QObject::connect(m_stateconnect2RM, &QAbstractState::entered, this, &cSEC1000dServer::doConnect2RM);
    QObject::connect(m_stateconnect2RMError, &QAbstractState::entered, this, &cSEC1000dServer::connect2RMError);
    QObject::connect(m_stateSendRMIdentAndRegister, &QAbstractState::entered, this, &cSEC1000dServer::doIdentAndRegister);
    QObject::connect(stateFINISH, &QAbstractState::entered, this, &cSEC1000dServer::doCloseServer);

    m_pInitializationMachine->start();
}

cSEC1000dServer::~cSEC1000dServer()
{
    delete m_pECalcSettings;
    delete m_pInputSettings;
    delete m_pStatusInterface;
    delete m_pSystemInterface;
    delete m_pECalculatorInterface;
    delete m_pSystemInfo;
    delete m_pRMConnection;

    AbstractDeviceNodeSecPtr deviceNode = m_deviceNodeFactory->getSecDeviceNode();
    deviceNode->close();
    close(pipeFileDescriptorSec1000[0]);
    close(pipeFileDescriptorSec1000[1]);
}

QString cSEC1000dServer::getServerVersion()
{
    return QString("%1 %2").arg(ServerName, ServerVersion);
}

QString cSEC1000dServer::getSecDeviceNode()
{
    return m_settings->getFpgaSettings()->getSecDeviceNode();
}

void cSEC1000dServer::doConfiguration(int ecUnitCount)
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
        ServerParams params = m_settings->getServerParams();

        // we want to initialize all settings first
        m_pECalcSettings = new SecCalculatorSettings(&m_xmlConfigReader, ecUnitCount);
        connect(&m_xmlConfigReader,&Zera::XMLConfig::cReader::valueChanged,m_pECalcSettings,&SecCalculatorSettings::configXMLInfo);
        m_pInputSettings = new SecInputSettings(&m_xmlConfigReader);
        connect(&m_xmlConfigReader,&Zera::XMLConfig::cReader::valueChanged,m_pInputSettings,&SecInputSettings::configXMLInfo);

        if(!m_xmlConfigReader.loadXMLFile(params.xmlFile)) {
            qCritical("Abort: Could not open xml file '%s", qPrintable(params.xmlFile));
            emit abortInit();
        }
    }
}

void cSEC1000dServer::earlySetup()
{
    m_pSystemInfo = new Sec1000SystemInfo();

    connectProtoConnectionSignals();

    m_scpiConnectionList.append(this); // the server itself has some commands
    m_scpiConnectionList.append(m_pStatusInterface = new Sec1000StatusInterface(m_scpiInterface));
    m_scpiConnectionList.append(m_pSystemInterface = new cSystemInterface(m_scpiInterface, this, m_pSystemInfo));
    m_scpiConnectionList.append(m_pECalculatorInterface = new SecGroupResourceAndInterface(m_scpiInterface,
                                                                                           m_pECalcSettings,
                                                                                           m_pInputSettings,
                                                                                           SigHandler,
                                                                                           m_deviceNodeFactory));

    m_resourceList.append(m_pECalculatorInterface); // all our resources
    m_ECalculatorChannelList = m_pECalculatorInterface->getECalcChannelList();
}

void cSEC1000dServer::doSetupServer()
{
    qInfo("Starting doSetupServer");
    QString deviceNodeName = getSecDeviceNode(); // we try to open the sec device
    AbstractDeviceNodeSecPtr deviceNode = m_deviceNodeFactory->getSecDeviceNode();
    if (deviceNode->open(deviceNodeName) < 0) {
        qCritical("Abort, could not poen device node %s", qPrintable(deviceNodeName));
        emit abortInit();
    }
    else
    {
        earlySetup();

        SECServer = this;

        initSCPIConnections();

        EthSettings *ethSettings = m_settings->getEthSettings();

        sigActionSec1000.sa_handler = &SigHandler; // signal handler einrichten
        sigemptyset(&sigActionSec1000.sa_mask);
        sigActionSec1000. sa_flags = SA_RESTART;
        sigActionSec1000.sa_restorer = NULL;
        sigaction(SIGIO, &sigActionSec1000, NULL); // handler für sigio definieren
        deviceNode->enableFasync();
        // our resource mananager connection must be opened after configuration is done
        m_pRMConnection = new RMConnection(ethSettings->getRMIPadr(),
                                           ethSettings->getPort(EthSettings::resourcemanager),
                                           m_tcpNetworkFactory);
        // so we must complete our state machine here
        m_retryRMConnect = 100;
        m_retryTimer.setSingleShot(true);
        connect(&m_retryTimer, &QTimer::timeout, this, &cSEC1000dServer::sigServerIsSetUp);

        m_stateconnect2RM->addTransition(m_pRMConnection, &RMConnection::connected, m_stateSendRMIdentAndRegister);
        m_stateconnect2RM->addTransition(m_pRMConnection, &RMConnection::connectionRMError, m_stateconnect2RMError);
        m_stateconnect2RMError->addTransition(this, &cSEC1000dServer::sigServerIsSetUp, m_stateconnect2RM);

        emit sigServerIsSetUp(); // so we enter state machine's next state
    }
}

void cSEC1000dServer::doCloseServer()
{
    QCoreApplication::instance()->exit(-1);
}

void cSEC1000dServer::doConnect2RM()
{
    qInfo("Starting doConnect2RM");
    m_pRMConnection->connect2RM();
}

void cSEC1000dServer::connect2RMError()
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

void cSEC1000dServer::doIdentAndRegister()
{
    qInfo("Starting doIdentAndRegister");
    m_pRMConnection->SendIdent(getName());
    for (int i = 0; i < m_resourceList.count(); i++) {
        cResource *res = m_resourceList.at(i);
        connect(m_pRMConnection, &RMConnection::rmAck, res, &cResource::resourceManagerAck);
        EthSettings *ethSettings = m_settings->getEthSettings();
        res->registerResource(m_pRMConnection, ethSettings->getPort(EthSettings::protobufserver));
        connect(res, &cResource::registerRdy, this, &cSEC1000dServer::onResourceReady);
    }
    m_pendingResources = m_resourceList.count();
}

void cSEC1000dServer::onResourceReady()
{
    Q_ASSERT(m_pendingResources > 0);
    m_pendingResources--;
    disconnect(static_cast<cResource*>(sender()), &cResource::registerRdy, this, &cSEC1000dServer::onResourceReady);
    if(m_pendingResources == 0) {
        EthSettings *ethSettings = m_settings->getEthSettings();
        m_protoBufServer.startServer(ethSettings->getPort(EthSettings::protobufserver));
    }
}

void cSEC1000dServer::onProtobufDisconnect(VeinTcp::TcpPeer* peer)
{
    if(!m_pECalculatorInterface->freeChannelsForThisPeer(peer))
        qWarning("Client disconnected. But SEC resources could not be freed!");
}


static inline int calcByteCountFromEcChannels(int ecChannelCount)
{
    int byteCount = ecChannelCount / 2;
    if ((ecChannelCount & 0b1) != 0)
        byteCount++;
    return byteCount;
}

static inline int calc32BitStuffedCountFromByteCount(int byteCount)
{
    int count32BitAcesses = byteCount / 4;
    if ((byteCount & 0b11) != 0)
        count32BitAcesses++;
    return count32BitAcesses * 4;
}

void cSEC1000dServer::SECIntHandler(int)
{
    char dummy[2];
    read(pipeFileDescriptorSec1000[0], dummy, 1); // first we read the pipe

    int ecChannelCount = m_ECalculatorChannelList.count();
    int bytesToReadForChannelRequiringHalfByte = calcByteCountFromEcChannels(ecChannelCount);
    int bytesToRead32BitStuffed = calc32BitStuffedCountFromByteCount(bytesToReadForChannelRequiringHalfByte);

    AbstractDeviceNodeSecPtr deviceNode = m_deviceNodeFactory->getSecDeviceNode();
    deviceNode->lseek(m_pECalcSettings->getIrqAdress()+4); // first word is interrupt collection word so the dedicated words have +4 offset
    QByteArray interruptREGS(bytesToRead32BitStuffed, 0);
    deviceNode->read(interruptREGS.data(), bytesToRead32BitStuffed);

    for (int channel = 0; channel < ecChannelCount; channel++) {
        quint8 irqMaskByteVal = interruptREGS[channel/2];
        quint8 irqRegVal;
        if((channel & 0b1) == 0)
            irqRegVal = irqMaskByteVal & 0x0F;
        else
            irqRegVal = (irqMaskByteVal & 0xF0) >> 4;
        m_ECalculatorChannelList[channel]->setIntReg(irqRegVal);
    }
}
