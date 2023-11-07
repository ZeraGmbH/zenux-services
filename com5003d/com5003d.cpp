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
#include <syslog.h>

#include "com5003dglobal.h"
#include "com5003d.h"
#include "pcbserver.h"
#include "systeminfo.h"
#include "adjustment.h"
#include "rmconnection.h"
#include "atmel.h"
#include "atmelwatcher.h"
#include "fingroupresourceandinterface.h"
#include "hkingroupresourceandinterface.h"
#include "samplinginterface.h"
#include "scingroupresourceandinterface.h"
#include "senseinterface.h"
#include "foutgroupresourceandinterface.h"
#include "statusinterface.h"
#include "systeminterface.h"
#include "debugsettings.h"
#include "ethsettings.h"
#include "finsettings.h"
#include "fpgasettings.h"
#include "hkinsettings.h"
#include "i2csettings.h"
#include "sensesettings.h"
#include "samplingsettings.h"
#include "foutsettings.h"
#include "scinsettings.h"
#include <scpisingletonfactory.h>

#ifdef SYSTEMD_NOTIFICATION
#include <systemd/sd-daemon.h>
#endif

static ServerParams params {ServerName, ServerVersion, defaultXSDFile, "/etc/zera/com5003d/com5003d.xml"};

cCOM5003dServer::cCOM5003dServer() :
    cPCBServer(params, ScpiSingletonFactory::getScpiObj())
{
    m_pDebugSettings = nullptr;
    m_pI2CSettings = nullptr;
    m_pFPGASettings = nullptr;
    m_pSenseSettings = nullptr;
    m_pAtmelWatcher = nullptr;
    m_pStatusInterface = nullptr;
    m_pSystemInterface = nullptr;
    m_pSenseInterface = nullptr;
    m_pSystemInfo = nullptr;
    m_pAdjHandler = nullptr;
    m_pRMConnection = nullptr;

    m_pInitializationMachine = new QStateMachine(this);

    QState* stateCONF = new QState(); // we start from here
    QFinalState* stateFINISH = new QFinalState(); // and here we finish

    stateCONF->addTransition(this, SIGNAL(abortInit()),stateFINISH); // from anywhere we arrive here if some error

    QState* statexmlConfiguration = new QState(stateCONF); // we configure our server with xml file
    QState* stateprogAtmel = new QState(stateCONF); // maybe we have to update the atmel
    QState* statewait4Atmel = new QState(stateCONF); // we synchronize on atmel running
    QState* statesetupServer = new QState(stateCONF); // we setup our server now
    stateconnect2RM = new QState(stateCONF); // we connect to resource manager
    stateconnect2RMError = new QState(stateCONF);
    stateSendRMIdentandRegister = new QState(stateCONF); // we send ident. to rm and register our resources

    stateCONF->setInitialState(statexmlConfiguration);

    statexmlConfiguration->addTransition(&m_xmlConfigReader, SIGNAL(finishedParsingXML(bool)), stateprogAtmel);
    stateprogAtmel->addTransition(this, SIGNAL(atmelProgrammed()), statewait4Atmel);
    statewait4Atmel->addTransition(this, SIGNAL(atmelRunning()), statesetupServer);
    statesetupServer->addTransition(this, SIGNAL(serverSetup()), stateconnect2RM);

    m_pInitializationMachine->addState(stateCONF);
    m_pInitializationMachine->addState(stateFINISH);
    m_pInitializationMachine->setInitialState(stateCONF);

    QObject::connect(statexmlConfiguration, &QAbstractState::entered, this, &cCOM5003dServer::doConfiguration);
    QObject::connect(stateprogAtmel, &QAbstractState::entered, this, &cCOM5003dServer::programAtmelFlash);
    QObject::connect(statewait4Atmel, &QAbstractState::entered, this, &cCOM5003dServer::doWait4Atmel);
    QObject::connect(statesetupServer, &QAbstractState::entered, this, &cCOM5003dServer::doSetupServer);
    QObject::connect(stateconnect2RM, &QAbstractState::entered, this, &cCOM5003dServer::doConnect2RM);
    QObject::connect(stateconnect2RMError, &QAbstractState::entered, this, &cCOM5003dServer::connect2RMError);
    QObject::connect(stateSendRMIdentandRegister, &QAbstractState::entered, this, &cCOM5003dServer::doIdentAndRegister);
    QObject::connect(stateFINISH, &QAbstractState::entered, this, &cCOM5003dServer::doCloseServer);

    m_pInitializationMachine->start();
}


cCOM5003dServer::~cCOM5003dServer()
{
    if (m_pDebugSettings) delete m_pDebugSettings;
    if (m_pI2CSettings) delete m_pI2CSettings;
    if (m_pFPGASettings) delete m_pFPGASettings;
    if (m_pSenseSettings) delete m_pSenseSettings;
    if (m_foutSettings) delete m_foutSettings;
    if (m_finSettings) delete m_finSettings;
    if (m_pSCHeadSettings) delete m_pSCHeadSettings;
    if (m_pAtmelWatcher) delete m_pAtmelWatcher;
    if (m_pStatusInterface) delete m_pStatusInterface;
    if (m_pSystemInterface) delete m_pSystemInterface;
    if (m_pSenseInterface) delete m_pSenseInterface;
    if (m_foutInterface) delete m_foutInterface;
    if (m_pFRQInputInterface) delete m_pFRQInputInterface;
    if (m_pSCHeadInterface) delete m_pSCHeadInterface;
    if (m_hkInInterface) delete m_hkInInterface;
    if (m_pSystemInfo) delete m_pSystemInfo;
    if (m_pAdjHandler) delete m_pAdjHandler;
    if (m_pRMConnection) delete m_pRMConnection;
}


void cCOM5003dServer::doConfiguration()
{
    QStringList args;

    quint32 sigStart;

    args = QCoreApplication::instance()->arguments();
    if (args.count() != 2) // we want exactly 1 parameter
    {
        m_nerror = parameterError;
        emit abortInit();
    }
    else
    {

        m_nFPGAfd = open("/dev/zFPGA1reg",O_RDWR);
        lseek(m_nFPGAfd,0x0,0);
        sigStart = 0;
        write(m_nFPGAfd, &sigStart, 4);
        sigStart = 1;
        write(m_nFPGAfd, &sigStart, 4);

        if (m_xmlConfigReader.loadSchema(defaultXSDFile))
        {
            sigStart = 0;
            write(m_nFPGAfd, &sigStart, 4);

            // we want to initialize all settings first
            QString xmlConfigTopNode = "serviceconfig";
            m_pDebugSettings = new cDebugSettings(&m_xmlConfigReader, xmlConfigTopNode);
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_pDebugSettings,&cDebugSettings::configXMLInfo);
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, &m_ethSettings, &EthSettings::configXMLInfo);
            m_pI2CSettings = new cI2CSettings(&m_xmlConfigReader);
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_pI2CSettings,&cI2CSettings::configXMLInfo);
            m_pFPGASettings = new FPGASettings(&m_xmlConfigReader, xmlConfigTopNode);
            connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged, m_pFPGASettings, &FPGASettings::configXMLInfo);
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

            QString s = args.at(1);
            qDebug() << s;

            sigStart = 1;
            write(m_nFPGAfd, &sigStart, 4);
            if (m_xmlConfigReader.loadXMLFile(s)) { // the first parameter should be the filename
                sigStart = 0;
                write(m_nFPGAfd, &sigStart, 4);
                // xmlfile ok -> nothing to do .. the configreader will emit all configuration
                // signals and after this the finishedparsingXML signal
            }
            else {
                m_nerror = xmlfileError;
                emit abortInit();
            }
        }
        else
        {
            m_nerror = xsdfileError;
            emit abortInit();
        }

        close(m_nFPGAfd);

    }
}


void cCOM5003dServer::programAtmelFlash()
{
    Atmel::init(m_pI2CSettings->getDeviceNode(), m_pI2CSettings->getI2CAdress(i2cSettings::relaisCtrlI2cAddress), m_pDebugSettings->getDebugLevel());

    QFile atmelFile(atmelFlashfilePath);
    if (atmelFile.exists())
    {
        int fd;
        QString devNode;

        m_nerror = atmelProgError; // preset error

        devNode = m_pFPGASettings->getDeviceNode();
        syslog(LOG_INFO,"Starting programming atmel flash\n");

        if ( (fd = open(devNode.toLatin1().data(),O_RDWR)) < 0 )
        {
            syslog(LOG_ERR,"error opening fpga device: %s\n",devNode.toLatin1().data());
            emit abortInit();
        }
        else
        {
            quint32 pcbTestReg;
            if ( lseek(fd,0xffc,0) < 0 )
            {
                syslog(LOG_ERR,"error positioning fpga device: %s\n", devNode.toLatin1().data());
                syslog(LOG_ERR,"Programming atmel failed\n");
                close(fd);
                emit abortInit();
            }

            ssize_t r = read(fd, &pcbTestReg,4);
            syslog(LOG_INFO,"reading fpga adr 0xFFC = 0x%08X\n", pcbTestReg);
            if (r < 0 )
            {
                syslog(LOG_ERR,"error reading fpga device: %s\n", devNode.toLatin1().data());
                syslog(LOG_ERR,"Programming atmel failed\n");
                emit abortInit();
            }

            pcbTestReg |= 1 << (atmelResetBit-1); // set bit for atmel reset
            syslog(LOG_INFO,"writing fpga adr 0xFFC = 0x%08X\n", pcbTestReg);
            r = write(fd, &pcbTestReg,4);

            if (r < 0 )
            {
                syslog(LOG_ERR,"error writing fpga device: %s\n", devNode.toLatin1().data());
                syslog(LOG_ERR,"Programming atmel failed\n");
                emit abortInit();
            }

            usleep(100); // give atmel some time for reset

            pcbTestReg &= static_cast<quint32>(~(1 << (atmelResetBit-1))); // reset bit for atmel reset
            syslog(LOG_INFO,"writing fpga adr 0xffc = 0x%08X\n", pcbTestReg);
            r = write(fd, &pcbTestReg,4);
            close(fd);

            if (r < 0 )
            {
                syslog(LOG_ERR,"error writing fpga device: %s\n", devNode.toLatin1().data());
                syslog(LOG_ERR,"Programming atmel failed\n");
                emit abortInit();
            }

            // atmel is reset
            usleep(100000); // now we wait for 100ms so bootloader is running definitely

            // and start writing flash
            cIntelHexFileIO IntelHexData;
            if (IntelHexData.ReadHexFile(atmelFlashfilePath))
            {
               syslog(LOG_INFO,"Writing %s to atmel...\n", atmelFlashfilePath);
               if (Atmel::getInstance().bootloaderLoadFlash(IntelHexData) == ZeraMControllerIo::cmddone)
               {
                   syslog(LOG_INFO,"Programming atmel passed\n");

                   // we must restart atmel now
                   if (Atmel::getInstance().bootloaderStartProgram() == ZeraMControllerIo::cmddone)
                   {
                       syslog(LOG_INFO,"Restart atmel after programming done\n");
                       // once the job is done, we remove the file
                       if(!atmelFile.remove())
                           syslog(LOG_ERR,"Error deleting %s\n", atmelFlashfilePath);

                       emit atmelProgrammed();
                   }
                   else
                   {
                       syslog(LOG_ERR,"Restart atmel after programming failed\n");
                       emit abortInit();
                   }
               }
               else
               {
                   syslog(LOG_ERR,"error writing atmel flash\n");
                   syslog(LOG_ERR,"Programming atmel failed\n");
                   emit abortInit();
               }
            }
            else
            {
                syslog(LOG_ERR,"error reading hex file\n");
                syslog(LOG_ERR,"Programming atmel failed\n");
                emit abortInit();
            }
        }
    }

    else
        emit atmelProgrammed();
}


void cCOM5003dServer::doWait4Atmel()
{
    m_pAtmelWatcher = new cAtmelWatcher(m_pDebugSettings->getDebugLevel(), m_pFPGASettings->getDeviceNode(), 10000, 100);

    m_nerror = atmelError; // we preset error
    connect(m_pAtmelWatcher,&cAtmelWatcher::timeout,this,&cCOM5003dServer::abortInit);
    connect(m_pAtmelWatcher,&cAtmelWatcher::running,this,&cCOM5003dServer::atmelRunning);
    m_pAtmelWatcher->start();
}


void cCOM5003dServer::doSetupServer()
{
    Atmel::getInstance().setPLLChannel(1); // default channel m0 for pll control
    m_pSystemInfo = new cSystemInfo();
    m_pAdjHandler = new cAdjustment(m_pSystemInfo, m_pI2CSettings->getDeviceNode(), m_pI2CSettings->getI2CAdress(i2cSettings::flashlI2cAddress) );

    cPCBServer::setupServer(); // here our scpi interface gets instanciated, we need this for further steps

    scpiConnectionList.append(this); // the server itself has some commands
    scpiConnectionList.append(m_pStatusInterface = new cStatusInterface(getSCPIInterface(), m_pAdjHandler));
    scpiConnectionList.append(m_pSystemInterface = new cSystemInterface(this));
    scpiConnectionList.append(m_pSenseInterface = new cSenseInterface(this));
    scpiConnectionList.append(m_pSamplingInterface = new cSamplingInterface(getSCPIInterface(), m_pSamplingSettings));
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

    m_pAdjHandler->addAdjFlashObject(m_pSenseInterface); // we add the senseinterface to both
    m_pAdjHandler->addAdjXMLObject(m_pSenseInterface); // adjustment list (flash and xml)
    m_pAdjHandler->importJDataFlash(); // we read adjustmentdata at least once

    initSCPIConnections();

    myServer->startServer(m_ethSettings.getPort(EthSettings::protobufserver)); // and can start the server now
    if(m_ethSettings.isSCPIactive())
        m_pSCPIServer->listen(QHostAddress::AnyIPv4, m_ethSettings.getPort(EthSettings::scpiserver));

    // our resource mananager connection must be opened after configuration is done
    m_pRMConnection = new RMConnection(m_ethSettings.getRMIPadr(), m_ethSettings.getPort(EthSettings::resourcemanager));
    //connect(m_pRMConnection, SIGNAL(connectionRMError()), this, SIGNAL(abortInit()));
    // so we must complete our state machine here
    m_nRetryRMConnect = 100;
    m_retryTimer.setSingleShot(true);
    connect(&m_retryTimer, &QTimer::timeout, this, &cCOM5003dServer::serverSetup);

    stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connected()), stateSendRMIdentandRegister);
    stateconnect2RM->addTransition(m_pRMConnection, SIGNAL(connectionRMError()), stateconnect2RMError);
    stateconnect2RMError->addTransition(this, SIGNAL(serverSetup()), stateconnect2RM);

    emit serverSetup(); // so we enter state machine's next state
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
    m_nRetryRMConnect--;
    if (m_nRetryRMConnect == 0)
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







