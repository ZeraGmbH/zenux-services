#include "mt310s2systeminterface.h"
#include "mt310s2d.h"
#include "systeminfo.h"
#include "mt310s2senseinterface.h"
#include "micro-controller-io/atmel.h"
#include "zscpi_response_definitions.h"
#include <QJsonObject>

Mt310s2SystemInterface::Mt310s2SystemInterface(cMT310S2dServer *server, HotPluggableControllerContainerPtr hotPluggableControllerContainer) :
    ScpiConnection(server->getSCPIInterface()),
    m_pMyServer(server),
    m_hotPluggableControllerContainer(std::move(hotPluggableControllerContainer))
{
    if(m_hotPluggableControllerContainer)
        connect(m_hotPluggableControllerContainer.get(), &HotPluggableControllerContainer::sigControllersChanged,
                this, &Mt310s2SystemInterface::onHotPluggableContainersChanged);
}


void Mt310s2SystemInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes),"SERVER", SCPI::isQuery, m_pSCPIInterface, SystemSystem::cmdVersionServer);
    addDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes),"DEVICE", SCPI::isQuery, m_pSCPIInterface, SystemSystem::cmdVersionDevice);
    addDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes), "PCB", SCPI::isQuery | SCPI::isCmdwP, m_pSCPIInterface, SystemSystem::cmdVersionPCB, &m_allPCBVersion);
    addDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes), "CTRL", SCPI::isQuery, m_pSCPIInterface, SystemSystem::cmdVersionCTRL, &m_allCtrlVersion);
    addDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes), "FPGA", SCPI::isQuery, m_pSCPIInterface, SystemSystem::cmdVersionFPGA);
    addDelegate(QString("%1SYSTEM").arg(leadingNodes), "SERIAL", SCPI::isQuery | SCPI::isCmdwP , m_pSCPIInterface, SystemSystem::cmdSerialNumber);
    addDelegate(QString("%1SYSTEM:UPDATE:CONTROLER").arg(leadingNodes), "BOOTLOADER", SCPI::isCmd, m_pSCPIInterface, SystemSystem::cmdUpdateControlerBootloader);
    addDelegate(QString("%1SYSTEM:UPDATE:CONTROLER").arg(leadingNodes), "PROGRAM", SCPI::isCmd, m_pSCPIInterface, SystemSystem::cmdUpdateControlerProgram);
    addDelegate(QString("%1SYSTEM:UPDATE:CONTROLER").arg(leadingNodes), "FLASH", SCPI::isCmdwP, m_pSCPIInterface, SystemSystem::cmdUpdateControlerFlash);
    addDelegate(QString("%1SYSTEM:UPDATE:CONTROLER").arg(leadingNodes), "EEPROM", SCPI::isCmdwP, m_pSCPIInterface, SystemSystem::cmdUpdateControlerEEprom);
    addDelegate(QString("%1SYSTEM:ADJUSTMENT:FLASH").arg(leadingNodes), "WRITE", SCPI::isCmd, m_pSCPIInterface, SystemSystem::cmdAdjFlashWrite);
    addDelegate(QString("%1SYSTEM:ADJUSTMENT:FLASH").arg(leadingNodes), "READ", SCPI::isCmd, m_pSCPIInterface, SystemSystem::cmdAdjFlashRead);
    addDelegate(QString("%1SYSTEM:ADJUSTMENT").arg(leadingNodes), "XML", SCPI::isQuery | SCPI::isCmdwP, m_pSCPIInterface, SystemSystem::cmdAdjXMLImportExport);
    addDelegate(QString("%1SYSTEM:ADJUSTMENT:XML").arg(leadingNodes), "WRITE", SCPI::isCmdwP, m_pSCPIInterface, SystemSystem::cmdAdjXMLWrite);
    addDelegate(QString("%1SYSTEM:ADJUSTMENT:XML").arg(leadingNodes), "READ", SCPI::isCmdwP, m_pSCPIInterface, SystemSystem::cmdAdjXMLRead);
    addDelegate(QString("%1SYSTEM:ADJUSTMENT:FLASH").arg(leadingNodes), "CHKSUM", SCPI::isQuery, m_pSCPIInterface, SystemSystem::cmdAdjFlashChksum);
    addDelegate(QString("%1SYSTEM:INTERFACE").arg(leadingNodes), "READ", SCPI::isQuery, m_pSCPIInterface, SystemSystem::cmdInterfaceRead);
    addDelegate(QString("%1SYSTEM").arg(leadingNodes),"TESTMODE",SCPI::isCmdwP, m_pSCPIInterface, SystemSystem::cmdTestMode);
}

void Mt310s2SystemInterface::actualizeContollers(quint16 bitmaskAvailable)
{
    m_hotPluggableControllerContainer->startActualizeEmobControllers(bitmaskAvailable,
                                                                     m_pMyServer->m_pSenseSettings,
                                                                     10000);
}


void Mt310s2SystemInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SystemSystem::cmdVersionServer:
        protoCmd->m_sOutput = scpiReadServerVersion(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdVersionDevice:
        protoCmd->m_sOutput = m_ReadDeviceVersion(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdVersionPCB:
        protoCmd->m_sOutput = m_ReadWritePCBVersion(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdVersionCTRL:
        protoCmd->m_sOutput = scpiReadAllCTRLVersions(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdVersionFPGA:
        protoCmd->m_sOutput = m_ReadFPGAVersion(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdSerialNumber:
        protoCmd->m_sOutput = m_ReadWriteSerialNumber(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdUpdateControlerBootloader:
        protoCmd->m_sOutput = m_StartControlerBootloader(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdUpdateControlerProgram:
        protoCmd->m_sOutput = m_StartControlerProgram(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdUpdateControlerFlash:
        protoCmd->m_sOutput = m_LoadFlash(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdUpdateControlerEEprom:
        protoCmd->m_sOutput = m_LoadEEProm(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdAdjFlashWrite:
        protoCmd->m_sOutput = m_AdjFlashWrite(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdAdjFlashRead:
        protoCmd->m_sOutput = m_AdjFlashRead(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdAdjXMLImportExport:
        protoCmd->m_sOutput = m_AdjXmlImportExport(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdAdjXMLWrite:
        protoCmd->m_sOutput = m_AdjXMLWrite(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdAdjXMLRead:
        protoCmd->m_sOutput = m_AdjXMLRead(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdAdjFlashChksum:
        protoCmd->m_sOutput = m_AdjFlashChksum(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdInterfaceRead:
        protoCmd->m_sOutput = m_InterfaceRead(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdTestMode:
        protoCmd->m_sOutput = testMode(protoCmd->m_sInput);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

void Mt310s2SystemInterface::onHotPluggableContainersChanged()
{
    updateAllCtrlVersionsJson();
    updateAllPCBsVersion();
}


QString Mt310s2SystemInterface::scpiReadServerVersion(QString &sInput)
{
    QString s;
    cSCPICommand cmd = sInput;

    if ( cmd.isQuery() )
    {
        s = m_pMyServer->getVersion();
    }
    else
        s = ZSCPI::scpiAnswer[ZSCPI::nak];

    return s;
}


QString Mt310s2SystemInterface::m_ReadDeviceVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (m_pMyServer->m_pSystemInfo->dataRead())
            return m_pMyServer->m_pSystemInfo->getDeviceVersion();
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Mt310s2SystemInterface::m_ReadDeviceName(QString& sInput)
{
    QString s;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (m_pMyServer->m_pSystemInfo->dataRead())
            return m_pMyServer->m_pSystemInfo->getDeviceName();
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Mt310s2SystemInterface::m_ReadWritePCBVersion(QString &sInput)
{
    QString s;
    int ret = ZeraMControllerIo::cmdfault;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (m_pMyServer->m_pSystemInfo->dataRead()) {
            updateAllPCBsVersion();
            s = m_allPCBVersion.getString();
        }
        else
            s = ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
    {
        if (cmd.isCommand(1))
        {
            QString Version = cmd.getParam(0);
            ret = Atmel::getInstance().writePCBVersion(Version);
            m_pMyServer->m_pSystemInfo->getSystemInfo(); // read back info
        }

        m_genAnswer(ret, s);
    }

    return s;
}


QString Mt310s2SystemInterface::scpiReadAllCTRLVersions(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        if (m_pMyServer->m_pSystemInfo->dataRead()) {
            updateAllCtrlVersionsJson();
            return m_allCtrlVersion.getString();
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Mt310s2SystemInterface::m_ReadFPGAVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        if (m_pMyServer->m_pSystemInfo->dataRead())
            return m_pMyServer->m_pSystemInfo->getLCAVersion();
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }

    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Mt310s2SystemInterface::m_ReadWriteSerialNumber(QString &sInput)
{
    ZeraMControllerIo::atmelRM ret = ZeraMControllerIo::cmdfault;
    QString s;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        {
            if (m_pMyServer->m_pSystemInfo->dataRead())
                s = m_pMyServer->m_pSystemInfo->getSerialNumber();
            else
                s = ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
    }
    else
    {
        if (cmd.isCommand(1))
        {
            QString Serial = cmd.getParam(0);
            ret = Atmel::getInstance().writeSerialNumber(Serial);
            m_pMyServer->m_pSystemInfo->getSystemInfo(); // read back info
        }

        m_genAnswer(ret, s);
    }

    return s;
}


QString Mt310s2SystemInterface::m_StartControlerBootloader(QString& sInput)
{
    QString s;
    int ret = ZeraMControllerIo::cmdfault;
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1) && (cmd.getParam(0) == ""))
    {
        ret = Atmel::getInstance().startBootLoader();
    }
    m_genAnswer(ret, s);
    return s;
}


QString Mt310s2SystemInterface::m_StartControlerProgram(QString &sInput)
{
    QString s;
    int ret = ZeraMControllerIo::cmdfault;
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1) && (cmd.getParam(0) == ""))
    {
        ret = Atmel::getInstance().bootloaderStartProgram();
    }
    m_genAnswer(ret, s);
    return s;
}


QString Mt310s2SystemInterface::m_LoadFlash(QString &sInput)
{
    QString s;
    int ret = ZeraMControllerIo::cmdfault;
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1))
    {
        QString filename = cmd.getParam(0);
        cIntelHexFileIO IntelHexData;
        if (IntelHexData.ReadHexFile(filename))
        {
           ret = Atmel::getInstance().bootloaderLoadFlash(IntelHexData);
        }
        else
            ret = ZeraMControllerIo::cmdexecfault;
    }
    m_genAnswer(ret, s);
    return s;
}


QString Mt310s2SystemInterface::m_LoadEEProm(QString &sInput)
{
    QString s;
    int ret = 1;
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1))
    {
        QString filename = cmd.getParam(0);
        cIntelHexFileIO IntelHexData;
        if (IntelHexData.ReadHexFile(filename))
        {
            ret = Atmel::getInstance().bootloaderLoadEEprom(IntelHexData);
        }
        else
            ret = ZeraMControllerIo::cmdexecfault;
    }
    m_genAnswer(ret, s);
    return s;
}


QString Mt310s2SystemInterface::m_AdjFlashWrite(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isCommand(1) && (cmd.getParam(0) == ""))
    {
        bool enable;
        if (Atmel::getInstance().hasPermission(enable))
        {
            if (enable)
            {
                if (m_pMyServer->m_pSenseInterface->exportAdjFlash())
                    return ZSCPI::scpiAnswer[ZSCPI::ack];
                else
                    return ZSCPI::scpiAnswer[ZSCPI::errexec];
            }
            else
                return ZSCPI::scpiAnswer[ZSCPI::erraut];
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }

    return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Mt310s2SystemInterface::m_AdjFlashRead(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1) && (cmd.getParam(0) == ""))
    {
        if (m_pMyServer->m_pSenseInterface->importAdjFlash())
            return ZSCPI::scpiAnswer[ZSCPI::ack];
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }

    return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Mt310s2SystemInterface::m_AdjXmlImportExport(QString &sInput)
{
    QString s;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        s = m_pMyServer->m_pSenseInterface->exportXMLString(-1);
        s.replace("\n","");
    }
    else
    {
        bool enable;
        if (Atmel::getInstance().hasPermission(enable))
        {
            if (enable)
            {
                QString XML = cmd.getParam();
                if (!m_pMyServer->m_pSenseInterface->importAdjXMLString(XML))
                    s = ZSCPI::errxml;
                else
                {
                    m_pMyServer->m_pSenseInterface->m_ComputeSenseAdjData();
                    if (!m_pMyServer->m_pSenseInterface->exportAdjFlash())
                        s = ZSCPI::scpiAnswer[ZSCPI::errexec];
                    else
                        s = ZSCPI::scpiAnswer[ZSCPI::ack];
                }
            }
            else
                s = ZSCPI::scpiAnswer[ZSCPI::erraut];
        }
        else
            s = ZSCPI::scpiAnswer[ZSCPI::errexec];
    }

    return s;
}


QString Mt310s2SystemInterface::m_AdjXMLWrite(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1))
    {
        QString filename = cmd.getParam(0);
        if (m_pMyServer->m_pSenseInterface->exportAdTojXMLFile(filename))
            return ZSCPI::scpiAnswer[ZSCPI::ack];
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Mt310s2SystemInterface::m_AdjXMLRead(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1))
    {
        bool enable;
        if (Atmel::getInstance().hasPermission(enable))
        {
            if (enable)
            {
                QString filename = cmd.getParam(0);
                if (m_pMyServer->m_pSenseInterface->importAdjXMLFile(filename))
                    return ZSCPI::scpiAnswer[ZSCPI::ack];
                else
                    return ZSCPI::scpiAnswer[ZSCPI::errexec];
            }
            else
                return ZSCPI::scpiAnswer[ZSCPI::erraut];
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }

    return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Mt310s2SystemInterface::m_AdjFlashChksum(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        QString s = QString("0x%1").arg(m_pMyServer->m_pSenseInterface->getChecksum()); // hex output
        return s;
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Mt310s2SystemInterface::m_InterfaceRead(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        QString s;
        m_pSCPIInterface->exportSCPIModelXML(s);
        return s;
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Mt310s2SystemInterface::testMode(QString &Input)
{
    cSCPICommand cmd = Input;
    quint32 modeBits = cmd.getParam(0).toUInt();
    return pAtmelSys->enableTestMode(modeBits)==ZeraMControllerIo::cmddone ? ZSCPI::scpiAnswer[ZSCPI::ack] : ZSCPI::scpiAnswer[ZSCPI::errexec];
}

void Mt310s2SystemInterface::updateAllCtrlVersionsJson()
{
    QJsonObject object;
    object.insert("Relay controller version", QJsonValue::fromVariant(m_pMyServer->m_pSystemInfo->getCTRLVersion()));
    object.insert("System controller version", QJsonValue::fromVariant(m_pMyServer->m_pSystemInfo->getSysCTRLVersion()));
    QVector<AtmelCommonVersionsPtr> hotpluggableControllers = m_hotPluggableControllerContainer->getCurrentControllers();
    for(auto controller : hotpluggableControllers) {
        QString version;
        controller->readCTRLVersion(version);
        object.insert("Emob controller version", QJsonValue::fromVariant(version));
    }
    QJsonDocument doc(object);
    m_allCtrlVersion = doc.toJson(QJsonDocument::Compact);
}

void Mt310s2SystemInterface::updateAllPCBsVersion()
{
    QJsonObject object;
    object.insert("Relay PCB version", QJsonValue::fromVariant(m_pMyServer->m_pSystemInfo->getPCBVersion()));
    object.insert("System PCB version", QJsonValue::fromVariant(m_pMyServer->m_pSystemInfo->getSysPCBVersion()));
    QVector<AtmelCommonVersionsPtr> hotpluggableControllers = m_hotPluggableControllerContainer->getCurrentControllers();
    for(auto controller : hotpluggableControllers) {
        QString version;
        controller->readPCBVersion(version);
        object.insert("Emob PCB version", QJsonValue::fromVariant(version));
    }
    QJsonDocument doc(object);
    m_allPCBVersion = doc.toJson(QJsonDocument::Compact);
}

void Mt310s2SystemInterface::m_genAnswer(int select, QString &answer)
{
    switch (select)
    {
    case ZeraMControllerIo::cmddone:
        answer = ZSCPI::scpiAnswer[ZSCPI::ack];
        break;
    case ZeraMControllerIo::cmdfault:
        answer = ZSCPI::scpiAnswer[ZSCPI::nak];
        break;
    case ZeraMControllerIo::cmdexecfault:
        answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
        break;
    }
}
