#include <QStateMachine>
#include <QState>
#include <QFile>
#include <QFinalState>
#include <QStringList>
#include <QDebug>
#include <QHostAddress>
#include <xmlconfigreader.h>
#include <QCoreApplication>
#include <xiqnetserver.h>
#include <unistd.h>
#include <fcntl.h>
#include "com5003dglobal.h"
#include "com5003d.h"
#include "pcbserver.h"
#include "systeminfo.h"
#include "rmconnection.h"
#include "fingroupresourceandinterface.h"
#include "hkingroupresourceandinterface.h"
#include "samplinginterface.h"
#include "scingroupresourceandinterface.h"
#include "com5003senseinterface.h"
#include "foutgroupresourceandinterface.h"
#include "statusinterface.h"
#include "com5003systeminterface.h"
#include "ethsettings.h"
#include "finsettings.h"
#include "fpgasettings.h"
#include "hkinsettings.h"
#include "sensesettings.h"
#include "samplingsettings.h"
#include "foutsettings.h"
#include "scinsettings.h"
#include <scpisingletonfactory.h>

#ifdef SYSTEMD_NOTIFICATION
#include <systemd/sd-daemon.h>
#endif

ServerParams cCOM5003dServer::defaultParams {ServerName, ServerVersion, "/etc/zera/com5003d/com5003d.xsd", "/etc/zera/com5003d/com5003d.xml"};

cCOM5003dServer::cCOM5003dServer(std::shared_ptr<SettingsForDeviceServer> settings, AbstractFactoryI2cCtrlPtr ctrlFactory) :
    cPCBServer(settings->getServerParams(), ScpiSingletonFactory::getScpiObj()),
    m_settings(settings),
    m_ctrlFactory(ctrlFactory)
{
    m_pInitializationMachine = new QStateMachine(this);

    QState* stateCONF = new QState(); // we start from here
    QFinalState* stateFINISH = new QFinalState(); // and here we finish

    stateCONF->addTransition(this, SIGNAL(abortInit()),stateFINISH); // from anywhere we arrive here if some error

    QState* statexmlConfiguration = new QState(stateCONF); // we configure our server with xml file
    QState* stateprogAtmel = new QState(stateCONF); // maybe we have to update the atmel
    QState* statewait4Atmel = new QState(stateCONF); // we synchronize on atmel running
    QState* statesetupServer = new QState(stateCONF); // we setup our server now
    m_stateconnect2RM = new QState(stateCONF); // we connect to resource manager
    m_stateconnect2RMError = new QState(stateCONF);
    m_stateSendRMIdentAndRegister = new QState(stateCONF); // we send ident. to rm and register our resources

    stateCONF->setInitialState(statexmlConfiguration);

    statexmlConfiguration->addTransition(&m_xmlConfigReader, SIGNAL(finishedParsingXML(bool)), stateprogAtmel);
    stateprogAtmel->addTransition(this, SIGNAL(atmelProgrammed()), statewait4Atmel);
    statewait4Atmel->addTransition(this, SIGNAL(atmelRunning()), statesetupServer);
    statesetupServer->addTransition(this, SIGNAL(sigServerIsSetUp()), m_stateconnect2RM);

    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);

    QObject::connect(statexmlConfiguration, &QAbstractState::entered, this, &cCOM5003dServer::doConfiguration);
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
    delete m_fpgaSettings;
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
    return m_fpgaSettings->getCtrlDeviceNode();
}

void cCOM5003dServer::setupMicroControllerIo()
{
    PermissionFunctions::setPermissionCtrlFactory(m_ctrlFactory);
    m_ctrlHeartbeatWait = m_ctrlFactory->createCtrlHeartbeatWait(getCtrlDeviceNode());
}

void cCOM5003dServer::doConfiguration()
{
    // What is this m_nFPGAfd I/O???
    // Once known and still needed: Use getCtrlDeviceNode()
    m_nFPGAfd = open("/dev/zFPGA1reg",O_RDWR);
    lseek(m_nFPGAfd,0x0,0);
    quint32 sigStart = 0;
    write(m_nFPGAfd, &sigStart, 4);
    sigStart = 1;
    write(m_nFPGAfd, &sigStart, 4);

    if (m_xmlConfigReader.loadSchema(m_params.xsdFile)) {
        sigStart = 0;
        write(m_nFPGAfd, &sigStart, 4);

        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, &m_ethSettings, &EthSettings::configXMLInfo);

        m_fpgaSettings = new FPGASettings(&m_xmlConfigReader);
        connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_fpgaSettings, &FPGASettings::configXMLInfo);
        m_pSenseSettings = new cSenseSettings(&m_xmlConfigReader, 6);
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

        sigStart = 1;
        write(m_nFPGAfd, &sigStart, 4);
        if (m_xmlConfigReader.loadXMLFile(m_params.xmlFile)) {
            setupMicroControllerIo();

            sigStart = 0;
            write(m_nFPGAfd, &sigStart, 4);
            // xmlfile ok -> nothing to do .. the configreader will emit all configuration
            // signals and after this the finishedparsingXML signal
        }
        else {
            qCritical("Abort: Could not open xml file '%s", qPrintable(m_params.xmlFile));
            emit abortInit();
        }
    }
    else {
        qCritical("Abort: Could not open xsd file '%s", qPrintable(m_params.xsdFile));
        emit abortInit();
    }
    close(m_nFPGAfd);
}

#define atmelResetBit 16
#define atmelFlashfilePath "/opt/zera/bin/atmel-com5003.hex"

void cCOM5003dServer::programAtmelFlash()
{
    QFile atmelFile(atmelFlashfilePath);
    if (atmelFile.exists())
    {
        int fd;
        QString devNode;

        m_nerror = atmelProgError; // preset error

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
            if (IntelHexData.ReadHexFile(atmelFlashfilePath))
            {
               qInfo("Writing %s to atmel...", atmelFlashfilePath);
                if (m_ctrlFactory->getBootloaderController()->bootloaderLoadFlash(IntelHexData) == ZeraMControllerIo::cmddone)
               {
                   qInfo("Programming atmel passed");

                   // we must restart atmel now
                   if (m_ctrlFactory->getBootloaderController()->bootloaderStartProgram() == ZeraMControllerIo::cmddone)
                   {
                       qInfo("Restart atmel after programming done");
                       // once the job is done, we remove the file
                       if(!atmelFile.remove())
                           qCritical("Error deleting %s", atmelFlashfilePath);

                       emit atmelProgrammed();
                   }
                   else
                   {
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
    m_nerror = atmelError; // we preset error
    connect(m_ctrlHeartbeatWait.get(), &AbstractCtrlHeartbeatWait::sigTimeout,
            this, &cCOM5003dServer::abortInit);
    connect(m_ctrlHeartbeatWait.get(), &AbstractCtrlHeartbeatWait::sigRunning,
            this, &cCOM5003dServer::atmelRunning);
    m_ctrlHeartbeatWait->start();
}


void cCOM5003dServer::doSetupServer()
{
    m_ctrlFactory->getPllController()->setPLLChannel(1); // default channel m0 for pll control
    m_pSystemInfo = new SystemInfo(m_ctrlFactory);

    setupServer(); // here our scpi interface gets instanciated, we need this for further steps

    // our resource mananager connection must be opened after configuration is done
    m_pRMConnection = new RMConnection(m_ethSettings.getRMIPadr(), m_ethSettings.getPort(EthSettings::resourcemanager));

    scpiConnectionList.append(this); // the server itself has some commands
    scpiConnectionList.append(m_pSenseInterface = new Com5003SenseInterface(getSCPIInterface(),
                                                                            m_settings->getI2cSettings(),
                                                                            m_pRMConnection,
                                                                            &m_ethSettings,
                                                                            m_pSenseSettings,
                                                                            m_pSystemInfo,
                                                                            m_ctrlFactory));
    scpiConnectionList.append(m_pStatusInterface = new cStatusInterface(getSCPIInterface(), m_pSenseInterface, m_ctrlFactory));
    scpiConnectionList.append(m_pSystemInterface = new Com5003SystemInterface(this, m_pSystemInfo, m_pSenseInterface, m_ctrlFactory));
    scpiConnectionList.append(m_pSamplingInterface = new cSamplingInterface(getSCPIInterface(), m_pSamplingSettings, m_ctrlFactory));
    scpiConnectionList.append(m_foutInterface = new FOutGroupResourceAndInterface(getSCPIInterface(), m_foutSettings));
    scpiConnectionList.append(m_pFRQInputInterface = new FInGroupResourceAndInterface(getSCPIInterface(), m_finSettings));
    scpiConnectionList.append(m_pSCHeadInterface = new ScInGroupResourceAndInterface(getSCPIInterface(), m_pSCHeadSettings));
    scpiConnectionList.append(m_hkInInterface = new HkInGroupResourceAndInterface(getSCPIInterface(), m_hkInSettings));

    resourceList.append(m_pSenseInterface); // all our resources
    resourceList.append(m_pSamplingInterface);
    resourceList.append(m_foutInterface);
    resourceList.append(m_pFRQInputInterface);
    resourceList.append(m_pSCHeadInterface);
    resourceList.append(m_hkInInterface);

    m_pSenseInterface->importAdjFlash(); // we read adjustmentdata at least once

    initSCPIConnections();

    m_myServer->startServer(m_ethSettings.getPort(EthSettings::protobufserver)); // and can start the server now
    if(m_ethSettings.isSCPIactive())
        m_pSCPIServer->listen(QHostAddress::AnyIPv4, m_ethSettings.getPort(EthSettings::scpiserver));

    //connect(m_pRMConnection, SIGNAL(connectionRMError()), this, SIGNAL(abortInit()));
    // so we must complete our state machine here
    m_retryRMConnect = 100;
    m_retryTimer.setSingleShot(true);
    connect(&m_retryTimer, &QTimer::timeout, this, &cCOM5003dServer::sigServerIsSetUp);

    m_stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connected()), m_stateSendRMIdentAndRegister);
    m_stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connectionRMError()), m_stateconnect2RMError);
    m_stateconnect2RMError->addTransition(this, SIGNAL(sigServerIsSetUp()), m_stateconnect2RM);

    emit sigServerIsSetUp(); // so we enter state machine's next state
}


void cCOM5003dServer::doCloseServer()
{
    QCoreApplication::instance()->exit(m_nerror);
}


void cCOM5003dServer::doConnect2RM()
{
    m_nerror = rmConnectionError; // preset error condition
    m_pRMConnection->connect2RM();
}


void cCOM5003dServer::connect2RMError()
{
    m_retryRMConnect--;
    if (m_retryRMConnect == 0)
        emit abortInit();
    else
        m_retryTimer.start(200);
}


void cCOM5003dServer::doIdentAndRegister()
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
