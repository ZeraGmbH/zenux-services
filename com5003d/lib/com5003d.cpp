#include "com5003d.h"
#include "com5003dglobal.h"
#include "com5003systeminterface.h"
#include "com5003senseinterface.h"
#include "fingroupresourceandinterface.h"
#include "firmwareupdatenotificationfile.h"
#include "hkingroupresourceandinterface.h"
#include "samplinginterface.h"
#include "scingroupresourceandinterface.h"
#include "foutgroupresourceandinterface.h"
#include "servicestatusinterface.h"
#include <QStateMachine>
#include <QState>
#include <QFile>
#include <QFinalState>
#include <QCoreApplication>
#include <unistd.h>
#include <fcntl.h>

const ServerParams cCOM5003dServer::defaultParams {ServerName, ServerVersion, "/etc/zera/com5003d/com5003d.xsd", "/etc/zera/com5003d/com5003d.xml"};

cCOM5003dServer::cCOM5003dServer(SettingsContainerPtr settings,
                                 AbstractFactoryI2cCtrlPtr ctrlFactory,
                                 AbstractFactoryDeviceNodePcbPtr deviceNodeFactory,
                                 AbstractEepromI2cFactoryPtr adjMemFactory,
                                 VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                 AbstractChannelRangeFactoryPtr channelRangeFactory) :
    PCBServer(std::move(settings), tcpNetworkFactory),
    m_ctrlFactory(ctrlFactory),
    m_deviceNodeFactory(deviceNodeFactory),
    m_adjMemFactory(adjMemFactory)
{
    doConfiguration();
    init();
    earlySetup(channelRangeFactory);
}

void cCOM5003dServer::init()
{
    m_pInitializationMachine = new QStateMachine(this);

    QState* stateCONF = new QState(); // we start from here
    QFinalState* stateFINISH = new QFinalState(); // and here we finish

    stateCONF->addTransition(this, &cCOM5003dServer::abortInit, stateFINISH); // from anywhere we arrive here if some error

    QState* stateprogAtmel = new QState(stateCONF); // maybe we have to update the atmel
    QState* statewait4Atmel = new QState(stateCONF); // we synchronize on atmel running
    QState* statesetupServer = new QState(stateCONF); // we setup our server now
    m_stateconnect2RM = new QState(stateCONF); // we connect to resource manager
    m_stateconnect2RMError = new QState(stateCONF);
    m_stateSendRMIdentAndRegister = new QState(stateCONF); // we send ident. to rm and register our resources

    stateCONF->setInitialState(stateprogAtmel);

    stateprogAtmel->addTransition(this, &cCOM5003dServer::atmelProgrammed, statewait4Atmel);
    statewait4Atmel->addTransition(this, &cCOM5003dServer::atmelRunning, statesetupServer);
    statesetupServer->addTransition(this, &cCOM5003dServer::sigServerIsSetUp, m_stateconnect2RM);

    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);

    QObject::connect(stateprogAtmel, &QAbstractState::entered, this, &cCOM5003dServer::programAtmelFlash);
    QObject::connect(statewait4Atmel, &QAbstractState::entered, this, &cCOM5003dServer::doWait4Atmel);
    QObject::connect(statesetupServer, &QAbstractState::entered, this, &cCOM5003dServer::doSetupServer);
    QObject::connect(m_stateconnect2RM, &QAbstractState::entered, this, &cCOM5003dServer::doConnect2RM);
    QObject::connect(m_stateconnect2RMError, &QAbstractState::entered, this, &cCOM5003dServer::connect2RMError);
    QObject::connect(m_stateSendRMIdentAndRegister, &QAbstractState::entered, this, &cCOM5003dServer::doIdentAndRegister);
    QObject::connect(stateFINISH, &QAbstractState::entered, this, &cCOM5003dServer::doCloseServer);

    m_pInitializationMachine->start();
}

cCOM5003dServer::~cCOM5003dServer()
{
    delete m_pSenseSettings;
    delete m_foutSettings;
    delete m_finSettings;
    delete m_pSCHeadSettings;
    delete m_pStatusInterface;
    delete m_pSystemInterface;
    delete m_pSenseInterface;
    delete m_foutInterface;
    delete m_pFRQInputInterface;
    delete m_pSCHeadInterface;
    delete m_hkInInterface;
    delete m_pSystemInfo;
    delete m_pRMConnection;
}

QString cCOM5003dServer::getCtrlDeviceNode()
{
    return m_settings->getFpgaSettings()->getCtrlDeviceNode();
}

void cCOM5003dServer::setupMicroControllerIo()
{
    PermissionFunctions::setPermissionCtrlFactory(m_ctrlFactory);
    m_ctrlHeartbeatWait = m_ctrlFactory->createCtrlHeartbeatWait(getCtrlDeviceNode());
}

void cCOM5003dServer::doConfiguration()
{
    // What is this m_nFPGAfd I/O??? / Atmel reset???
    // Once known and still needed: Use getCtrlDeviceNode()
    m_nFPGAfd = open("/dev/zFPGA1reg",O_RDWR);
    lseek(m_nFPGAfd,0x0,0);
    quint32 sigStart = 0;
    write(m_nFPGAfd, &sigStart, 4);
    sigStart = 1;
    write(m_nFPGAfd, &sigStart, 4);

    // THE MOST UTTER UGLINESS FOUND SO FAR:
    // The delay caused by m_xmlConfigReader.loadSchema is mandatory
    // to bring up Atmel properly!!!
    ServerParams params = m_settings->getServerParams();
    qInfo("Loading schema...");
    if (m_xmlConfigReader.loadSchema(params.xsdFile)) {
        qInfo("Schema loaded.");
        sigStart = 0;
        write(m_nFPGAfd, &sigStart, 4);

        m_pSenseSettings = new cSenseSettings(&m_xmlConfigReader, 6);
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_pSenseSettings, &cSenseSettings::configXMLInfo);
        m_foutSettings = new FOutSettings(&m_xmlConfigReader);
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_foutSettings, &FOutSettings::configXMLInfo);
        m_finSettings = new FInSettings(&m_xmlConfigReader);
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_finSettings, &FInSettings::configXMLInfo);
        m_pSCHeadSettings = new ScInSettings(&m_xmlConfigReader);
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_pSCHeadSettings, &ScInSettings::configXMLInfo);
        m_hkInSettings = new HkInSettings(&m_xmlConfigReader);
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_hkInSettings, &HkInSettings::configXMLInfo);

        sigStart = 1;
        write(m_nFPGAfd, &sigStart, 4);
        // Same here (or even worse): When not loading XSD above, delay caused here
        // will get too short - WTF???
        // So for now measure both times by qInfo and maybe come back later...
        qInfo("Loading XML...");
        if (m_xmlConfigReader.loadXMLFile(params.xmlFile)) {
            qInfo("Loading XML loaded");
            setupMicroControllerIo();

            sigStart = 0;
            write(m_nFPGAfd, &sigStart, 4);
            // xmlfile ok -> nothing to do .. the configreader will emit all configuration
            // signals and after this the finishedparsingXML signal
        }
        else {
            qCritical("Abort: Could not open xml file '%s", qPrintable(params.xmlFile));
            emit abortInit();
        }
    }
    else {
        qCritical("Abort: Could not open xsd file '%s", qPrintable(params.xsdFile));
        emit abortInit();
    }
    close(m_nFPGAfd);
}

#define atmelResetBit 16
#define atmelFlashfilePath "/opt/zera/bin/atmel-com5003.hex"

void cCOM5003dServer::programAtmelFlash()
{
    QFile atmelFile(atmelFlashfilePath);
    if (atmelFile.exists()) { // remember: flash files are deleted after completion :((
        FirmwareUpdateNotificationFile notificationFile;

        int fd;
        QString devNode;

        devNode = getCtrlDeviceNode();
        qInfo("Starting programming atmel flash");

        if ( (fd = open(devNode.toLatin1().data(),O_RDWR)) < 0 ) {
            qCritical("Error opening FPGA device: %s",devNode.toLatin1().data());
            emit abortInit();
        }
        else {
            quint32 pcbTestReg;
            if ( lseek(fd,0xffc,0) < 0 ) {
                qCritical("Error positioning FPGA device: %s", devNode.toLatin1().data());
                qCritical("Programming atmel failed");
                close(fd);
                emit abortInit();
            }

            ssize_t r = read(fd, &pcbTestReg,4);
            qInfo("Reading FPGA adr 0xFFC = 0x%08X", pcbTestReg);
            if (r < 0 )
            {
                qCritical("Error reading FPGA device: %s", devNode.toLatin1().data());
                qCritical("Programming atmel failed");
                emit abortInit();
            }

            pcbTestReg |= 1 << (atmelResetBit-1); // set bit for atmel reset
            qInfo("Writing FPGA adr 0xFFC = 0x%08X", pcbTestReg);
            r = write(fd, &pcbTestReg,4);

            if (r < 0 )
            {
                qCritical("Error writing fpga device: %s", devNode.toLatin1().data());
                qCritical("Programming atmel failed");
                emit abortInit();
            }

            usleep(100); // give atmel some time for reset

            pcbTestReg &= static_cast<quint32>(~(1 << (atmelResetBit-1))); // reset bit for atmel reset
            qInfo("Writing FPGA adr 0xffc = 0x%08X", pcbTestReg);
            r = write(fd, &pcbTestReg,4);
            close(fd);

            if (r < 0 )
            {
                qCritical("Error writing FPGA device: %s", devNode.toLatin1().data());
                qCritical("Programming atmel failed");
                emit abortInit();
            }

            // atmel is reset
            usleep(100000); // now we wait for 100ms so bootloader is running definitely

            // and start writing flash
            cIntelHexFileIO IntelHexData;
            if (IntelHexData.ReadHexFile(atmelFlashfilePath)) {
                qInfo("Writing %s to atmel...", atmelFlashfilePath);
                I2cCtrlBootloaderPtr bootloader = m_ctrlFactory->getBootloaderController(AbstractFactoryI2cCtrl::CTRL_TYPE_RELAIS);
                if (bootloader->bootloaderLoadFlash(IntelHexData) == ZeraMControllerIo::cmddone) {
                    qInfo("Programming atmel passed");
                    // we must restart atmel now
                    if (bootloader->bootloaderStartProgram() == ZeraMControllerIo::cmddone) {
                        qInfo("Restart atmel after programming done");
                        // once the job is done, we remove the file
                        if(!atmelFile.remove())
                            qCritical("Error deleting %s", atmelFlashfilePath);

                        emit atmelProgrammed();
                    }
                    else {
                        qCritical("Restart atmel after programming failed");
                        emit abortInit();
                    }
                }
               else
               {
                   qCritical("Error writing atmel flash");
                   qCritical("Programming atmel failed");
                   emit abortInit();
               }
            }
            else
            {
                qCritical("Error reading hex file");
                qCritical("Programming atmel failed");
                emit abortInit();
            }
        }
    }

    else
        emit atmelProgrammed();
}


void cCOM5003dServer::doWait4Atmel()
{
    connect(m_ctrlHeartbeatWait.get(), &AbstractCtrlHeartbeatWait::sigTimeout,
            this, &cCOM5003dServer::abortInit);
    connect(m_ctrlHeartbeatWait.get(), &AbstractCtrlHeartbeatWait::sigRunning,
            this, &cCOM5003dServer::atmelRunning);
    m_ctrlHeartbeatWait->start();
}


void cCOM5003dServer::earlySetup(AbstractChannelRangeFactoryPtr channelRangeFactory)
{
    qInfo("Set initial PLL channel...");
    m_ctrlFactory->getPllController()->setPLLChannel(1); // default channel m0 for pll control
    qInfo("Initial PLL channel set");
    m_pSystemInfo = new SystemInfo(m_ctrlFactory);

    connectProtoConnectionSignals();

    // our resource mananager connection must be opened after configuration is done
    EthSettings *ethSettings = m_settings->getEthSettings();
    m_pRMConnection = new RMConnection(ethSettings->getRMIPadr(),
                                       ethSettings->getPort(EthSettings::resourcemanager),
                                       m_tcpNetworkFactory);

    m_scpiConnectionList.append(this); // the server itself has some commands
    I2cSettings *i2cSettings = m_settings->getI2cSettings();
    EepromI2cDeviceInterfacePtr eepromDev = m_adjMemFactory->createEeprom(
        {i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)},
        i2cSettings->getEepromByteSize());
    m_scpiConnectionList.append(m_pSenseInterface = new Com5003SenseInterface(m_scpiInterface,
                                                                              m_pSenseSettings,
                                                                              std::move(eepromDev),
                                                                              m_pSystemInfo,
                                                                              channelRangeFactory,
                                                                              m_ctrlFactory));
    m_scpiConnectionList.append(m_pStatusInterface = new ServiceStatusInterface(m_scpiInterface, m_pSenseInterface, m_ctrlFactory));
    m_scpiConnectionList.append(m_pSystemInterface = new Com5003SystemInterface(this, m_pSystemInfo, m_pSenseInterface, m_ctrlFactory));
    m_scpiConnectionList.append(m_pSamplingInterface = new cSamplingInterface(m_scpiInterface, m_settings->getSamplingSettings(), m_ctrlFactory));
    m_scpiConnectionList.append(m_foutInterface = new FOutGroupResourceAndInterface(m_scpiInterface, m_foutSettings));
    m_scpiConnectionList.append(m_pFRQInputInterface = new FInGroupResourceAndInterface(m_scpiInterface, m_finSettings));
    m_scpiConnectionList.append(m_pSCHeadInterface = new ScInGroupResourceAndInterface(m_scpiInterface, m_pSCHeadSettings));
    m_scpiConnectionList.append(m_hkInInterface = new HkInGroupResourceAndInterface(m_scpiInterface, m_hkInSettings));

    m_resourceList.append(m_pSenseInterface); // all our resources
    m_resourceList.append(m_pSamplingInterface);
    m_resourceList.append(m_foutInterface);
    m_resourceList.append(m_pFRQInputInterface);
    m_resourceList.append(m_pSCHeadInterface);
    m_resourceList.append(m_hkInInterface);
    qInfo("SCPI interfaces set.");
}

void cCOM5003dServer::doSetupServer()
{
    qInfo("Starting doSetupServer");

    initSCPIConnections();

    // so we must complete our state machine here
    m_retryRMConnect = 100;
    m_retryTimer.setSingleShot(true);
    connect(&m_retryTimer, &QTimer::timeout, this, &cCOM5003dServer::sigServerIsSetUp);

    m_stateconnect2RM->addTransition(m_pRMConnection, &RMConnection::connected, m_stateSendRMIdentAndRegister);
    m_stateconnect2RM->addTransition(m_pRMConnection, &RMConnection::connectionRMError, m_stateconnect2RMError);
    m_stateconnect2RMError->addTransition(this, &cCOM5003dServer::sigServerIsSetUp, m_stateconnect2RM);

    emit sigServerIsSetUp(); // so we enter state machine's next state
}


void cCOM5003dServer::doCloseServer()
{
    QCoreApplication::instance()->exit(-1);
}


void cCOM5003dServer::doConnect2RM()
{
    qInfo("Starting doConnect2RM");
    m_pRMConnection->connect2RM();
}


void cCOM5003dServer::connect2RMError()
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


void cCOM5003dServer::doIdentAndRegister()
{
    qInfo("Starting doIdentAndRegister");
    m_pRMConnection->SendIdent(getName());
    for (int i = 0; i < m_resourceList.count(); i++) {
        cResource *res = m_resourceList.at(i);
        connect(m_pRMConnection, &RMConnection::rmAck, res, &cResource::resourceManagerAck );
        EthSettings *ethSettings = m_settings->getEthSettings();
        res->registerResource(m_pRMConnection, ethSettings->getPort(EthSettings::protobufserver));
        connect(res, &cResource::registerRdy, this, &cCOM5003dServer::onResourceReady);
    }
    m_pendingResources = m_resourceList.count();
}

void cCOM5003dServer::onResourceReady()
{
    Q_ASSERT(m_pendingResources > 0);
    m_pendingResources--;
    disconnect(static_cast<cResource*>(sender()), &cResource::registerRdy, this, &cCOM5003dServer::onResourceReady);
    if(m_pendingResources == 0) {
        EthSettings *ethSettings = m_settings->getEthSettings();
        m_protoBufServer.startServer(ethSettings->getPort(EthSettings::protobufserver));
        openTelnetScpi();
    }
}
