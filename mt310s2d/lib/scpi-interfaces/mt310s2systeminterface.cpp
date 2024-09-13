#include "mt310s2systeminterface.h"
#include "accustatusflags.h"
#include "zscpi_response_definitions.h"
#include <QJsonObject>
#include <QDateTime>

Mt310s2SystemInterface::Mt310s2SystemInterface(cPCBServer *server,
                                               Mt310s2SystemInfo *systemInfo,
                                               cSenseSettings *senseSettings,
                                               SenseInterfaceCommon* senseInterface,
                                               AbstractFactoryI2cCtrlPtr ctrlFactory,
                                               HotPluggableControllerContainerPtr hotPluggableControllerContainer) :
    ScpiConnection(server->getSCPIInterface()),
    m_pMyServer(server),
    m_systemInfo(systemInfo),
    m_senseSettings(senseSettings),
    m_senseInterface(senseInterface),
    m_ctrlFactory(ctrlFactory),
    m_hotPluggableControllerContainer(std::move(hotPluggableControllerContainer))
{
    if(m_hotPluggableControllerContainer)
        connect(m_hotPluggableControllerContainer.get(), &HotPluggableControllerContainer::sigControllersChanged, this, [=]() {
            qInfo("Hot plug controllers changed.");
            onHotPluggablesChanged();
        });
}


void Mt310s2SystemInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes),"SERVER", SCPI::isQuery, m_pSCPIInterface, SystemSystem::cmdVersionServer);
    addDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes),"DEVICE", SCPI::isQuery, m_pSCPIInterface, SystemSystem::cmdVersionDevice);
    addDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes), "PCB", SCPI::isQuery, m_pSCPIInterface, SystemSystem::cmdVersionPCB, &m_allPCBVersion);
    addDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes), "CTRL", SCPI::isQuery, m_pSCPIInterface, SystemSystem::cmdVersionCTRL, &m_allCtrlVersion);
    addDelegate(QString("%1SYSTEM:VERSION").arg(leadingNodes), "FPGA", SCPI::isQuery, m_pSCPIInterface, SystemSystem::cmdVersionFPGA);
    addDelegate(QString("%1SYSTEM").arg(leadingNodes), "SERIAL", SCPI::isQuery | SCPI::isCmdwP , m_pSCPIInterface, SystemSystem::cmdSerialNumber);
    addDelegate(QString("%1SYSTEM:ADJUSTMENT:FLASH").arg(leadingNodes), "WRITE", SCPI::isCmd, m_pSCPIInterface, SystemSystem::cmdAdjFlashWrite);
    addDelegate(QString("%1SYSTEM:ADJUSTMENT:FLASH").arg(leadingNodes), "READ", SCPI::isCmd, m_pSCPIInterface, SystemSystem::cmdAdjFlashRead);
    addDelegate(QString("%1SYSTEM:ADJUSTMENT").arg(leadingNodes), "XML", SCPI::isQuery | SCPI::isCmdwP, m_pSCPIInterface, SystemSystem::cmdAdjXMLImportExport);
    // Obsolete???
        addDelegate(QString("%1SYSTEM:ADJUSTMENT:XML").arg(leadingNodes), "WRITE", SCPI::isCmdwP, m_pSCPIInterface, SystemSystem::cmdAdjXMLWrite);
        addDelegate(QString("%1SYSTEM:ADJUSTMENT:XML").arg(leadingNodes), "READ", SCPI::isCmdwP, m_pSCPIInterface, SystemSystem::cmdAdjXMLRead);
    // End Obsolete???
    addDelegate(QString("%1SYSTEM:ADJUSTMENT:FLASH").arg(leadingNodes), "CHKSUM", SCPI::isQuery, m_pSCPIInterface, SystemSystem::cmdAdjFlashChksum);
    addDelegate(QString("%1SYSTEM:INTERFACE").arg(leadingNodes), "READ", SCPI::isQuery, m_pSCPIInterface, SystemSystem::cmdInterfaceRead);
}

void Mt310s2SystemInterface::actualizeContollers(quint16 bitmaskAvailable)
{
    m_hotPluggableControllerContainer->startActualizeEmobControllers(bitmaskAvailable,
                                                                     m_senseSettings,
                                                                     10000);
}

void Mt310s2SystemInterface::onAccuStatusChanged(uint8_t status)
{
    bool accuPlugged = status & (1<<bp_Battery_Present);
    if(m_currAccuPlugged != accuPlugged) {
        m_currAccuPlugged = accuPlugged;
        QString pluggedString = m_currAccuPlugged ? "plugged" : "unplugged";
        qInfo("Accu was detected as %s.", qPrintable(pluggedString));
        onHotPluggablesChanged();
    }
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
        protoCmd->m_sOutput = scpiReadPCBVersion(protoCmd->m_sInput);
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
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

void Mt310s2SystemInterface::onHotPluggablesChanged()
{
    qInfo("Read versions of all hotpluggables...");
    updateAllCtrlVersionsJson();
    updateAllPCBsVersion();
    qInfo("Hotpluggables' versions were read.");
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
        if (m_systemInfo->dataRead())
            return m_systemInfo->getDeviceVersion();
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
        if (m_systemInfo->dataRead())
            return m_systemInfo->getDeviceName();
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Mt310s2SystemInterface::scpiReadPCBVersion(QString &sInput)
{
    QString s;
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        if (m_systemInfo->dataRead()) {
            updateAllPCBsVersion();
            s = m_allPCBVersion.getString();
        }
        else
            s = ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        s = ZSCPI::scpiAnswer[ZSCPI::nak];

    return s;
}


QString Mt310s2SystemInterface::scpiReadAllCTRLVersions(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        if (m_systemInfo->dataRead()) {
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
        if (m_systemInfo->dataRead())
            return m_systemInfo->getLCAVersion();
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
            if (m_systemInfo->dataRead())
                s = m_systemInfo->getSerialNumber();
            else
                s = ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
    }
    else
    {
        if (cmd.isCommand(1))
        {
            QString Serial = cmd.getParam(0);
            ret = m_ctrlFactory->getDeviceIdentController()->writeSerialNumber(Serial);
            m_systemInfo->getSystemInfo(); // read back info
        }

        m_genAnswer(ret, s);
    }

    return s;
}


QString Mt310s2SystemInterface::m_AdjFlashWrite(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isCommand(1) && (cmd.getParam(0) == ""))
    {
        bool enable;
        if (m_ctrlFactory->getPermissionCheckController()->hasPermission(enable))
        {
            if (enable)
            {
                if (m_senseInterface->exportAdjData(QDateTime::currentDateTime()))
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
        if (m_senseInterface->importAdjData())
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
    if (cmd.isQuery()) {
        s = m_senseInterface->exportXMLString(-1);
        s.replace("\n", "");
    }
    else {
        bool enable;
        if (m_ctrlFactory->getPermissionCheckController()->hasPermission(enable)) {
            if (enable) {
                QString XML = cmd.getParam();
                if (!m_senseInterface->importAdjXMLString(XML))
                    s = ZSCPI::scpiAnswer[ZSCPI::errxml];
                else
                {
                    m_senseInterface->computeSenseAdjData();
                    if (!m_senseInterface->exportAdjData(QDateTime::currentDateTime()))
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
        if (m_senseInterface->exportAdTojXMLFile(filename))
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
        if (m_ctrlFactory->getPermissionCheckController()->hasPermission(enable))
        {
            if (enable)
            {
                QString filename = cmd.getParam(0);
                if (m_senseInterface->importAdjXMLFile(filename))
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
        QString s = QString("0x%1").arg(m_senseInterface->getAdjChecksum()); // hex output
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

void Mt310s2SystemInterface::updateAllCtrlVersionsJson()
{
    QJsonObject object;
    object.insert("Relay controller version", QJsonValue::fromVariant(m_systemInfo->getCTRLVersion()));
    object.insert("System controller version", QJsonValue::fromVariant(m_systemInfo->getSysCTRLVersion()));
    QVector<I2cCtrlCommonInfoPtrShared> hotpluggableControllers = m_hotPluggableControllerContainer->getCurrentControllers();
    for(auto controller : hotpluggableControllers) {
        QString version;
        controller->readCTRLVersion(version);
        object.insert("Emob controller version", QJsonValue::fromVariant(version));
    }
    if(m_currAccuPlugged) {
        I2cCtrlCommonInfoPtrUnique controller = m_ctrlFactory->getCommonInfoController(AbstractFactoryI2cCtrl::CTRL_TYPE_ACCU);
        QString version;
        controller->readCTRLVersion(version);
        object.insert("Accu controller version", version);
    }
    QJsonDocument doc(object);
    m_allCtrlVersion = doc.toJson(QJsonDocument::Compact);
}

void Mt310s2SystemInterface::updateAllPCBsVersion()
{
    QJsonObject object;
    object.insert("Relay PCB version", QJsonValue::fromVariant(m_systemInfo->getPCBVersion()));
    object.insert("System PCB version", QJsonValue::fromVariant(m_systemInfo->getSysPCBVersion()));
    QVector<I2cCtrlCommonInfoPtrShared> hotpluggableControllers = m_hotPluggableControllerContainer->getCurrentControllers();
    for(auto controller : hotpluggableControllers) {
        QString version;
        controller->readPCBInfo(version);
        object.insert("Emob PCB version", QJsonValue::fromVariant(version));
    }
    if(m_currAccuPlugged) {
        I2cCtrlCommonInfoPtrUnique controller = m_ctrlFactory->getCommonInfoController(AbstractFactoryI2cCtrl::CTRL_TYPE_ACCU);
        QString version;
        controller->readPCBInfo(version);
        object.insert("Accu PCB version", version);
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
