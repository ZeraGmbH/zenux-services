#include "com5003systeminterface.h"
#include "systeminfo.h"
#include "commonscpimethods.h"
#include "zscpi_response_definitions.h"
#include <QDateTime>
#include <QJsonObject>

Com5003SystemInterface::Com5003SystemInterface(PCBServer *server,
                                               SystemInfo *systemInfo,
                                               SenseInterfaceCommon *senseInterface,
                                               AbstractFactoryI2cCtrlPtr ctrlFactory) :
    ScpiConnection(server->getSCPIInterface()),
    m_pMyServer(server),
    m_systemInfo(systemInfo),
    m_senseInterface(senseInterface),
    m_ctrlFactory(ctrlFactory)
{
}

void Com5003SystemInterface::initSCPIConnection(const QString &leadingNodes)
{
    const QString adjLeadNodes = appendTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SYSTEM:VERSION").arg(adjLeadNodes),"SERVER", SCPI::isQuery, m_scpiInterface, SystemSystem::cmdVersionServer);
    addDelegate(QString("%1SYSTEM:VERSION").arg(adjLeadNodes),"DEVICE", SCPI::isQuery, m_scpiInterface, SystemSystem::cmdVersionDevice);
    addDelegate(QString("%1SYSTEM:VERSION").arg(adjLeadNodes), "PCB", SCPI::isQuery, m_scpiInterface, SystemSystem::cmdVersionPCB, &m_allCtrlVersion); // why notification??
    addDelegate(QString("%1SYSTEM:VERSION").arg(adjLeadNodes), "CTRL", SCPI::isQuery, m_scpiInterface, SystemSystem::cmdVersionCTRL, &m_allCtrlVersion);
    addDelegate(QString("%1SYSTEM:EMOB").arg(adjLeadNodes), "CHANNEL", SCPI::isQuery, m_scpiInterface, SystemSystem::cmdGetChannels);
    addDelegate(QString("%1SYSTEM:VERSION").arg(adjLeadNodes), "FPGA", SCPI::isQuery, m_scpiInterface, SystemSystem::cmdVersionFPGA);
    addDelegate(QString("%1SYSTEM").arg(adjLeadNodes), "SERIAL", SCPI::isQuery | SCPI::isCmdwP , m_scpiInterface, SystemSystem::cmdSerialNumber);
    addDelegate(QString("%1SYSTEM:ADJUSTMENT:FLASH").arg(adjLeadNodes), "WRITE", SCPI::isCmd, m_scpiInterface, SystemSystem::cmdAdjFlashWrite);
    addDelegate(QString("%1SYSTEM:ADJUSTMENT:FLASH").arg(adjLeadNodes), "READ", SCPI::isCmd, m_scpiInterface, SystemSystem::cmdAdjFlashRead);
    addDelegate(QString("%1SYSTEM:ADJUSTMENT").arg(adjLeadNodes), "XML", SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, SystemSystem::cmdAdjXMLImportExport);
    // Obsolete???
        addDelegate(QString("%1SYSTEM:ADJUSTMENT:XML").arg(adjLeadNodes), "WRITE", SCPI::isCmdwP, m_scpiInterface, SystemSystem::cmdAdjXMLWrite);
        addDelegate(QString("%1SYSTEM:ADJUSTMENT:XML").arg(adjLeadNodes), "READ", SCPI::isCmdwP, m_scpiInterface, SystemSystem::cmdAdjXMLRead);
    // End Obsolete???
    addDelegate(QString("%1SYSTEM:ADJUSTMENT:FLASH").arg(adjLeadNodes), "CHKSUM", SCPI::isQuery, m_scpiInterface, SystemSystem::cmdAdjFlashChksum);
    addDelegate(QString("%1SYSTEM:INTERFACE").arg(adjLeadNodes), "READ", SCPI::isQuery, m_scpiInterface, SystemSystem::cmdInterfaceRead);
}


void Com5003SystemInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
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
        protoCmd->m_sOutput = CommonScpiMethods::handleScpiInterfaceRead(m_scpiInterface, protoCmd->m_sInput);
        break;
    case SystemSystem::cmdGetChannels:
        protoCmd->m_sOutput = "";
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


QString Com5003SystemInterface::scpiReadServerVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if ( cmd.isQuery() )
        return m_pMyServer->getVersion();
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Com5003SystemInterface::m_ReadDeviceVersion(QString &sInput)
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


QString Com5003SystemInterface::m_ReadDeviceName(QString& sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        if (m_systemInfo->dataRead())
            return m_systemInfo->getDeviceName();
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Com5003SystemInterface::scpiReadPCBVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        if (m_systemInfo->dataRead()) {
            updateAllPCBsVersion();
            return m_allPCBVersion.getString();
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Com5003SystemInterface::scpiReadAllCTRLVersions(QString &sInput)
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


QString Com5003SystemInterface::m_ReadFPGAVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        if (m_systemInfo->dataRead())
            return m_systemInfo->getLCAVersion();
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Com5003SystemInterface::m_ReadWriteSerialNumber(QString &sInput)
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


QString Com5003SystemInterface::m_AdjFlashWrite(QString &sInput)
{
    QString s;
    int ret = ZeraMControllerIo::cmdfault;
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1) && (cmd.getParam(0) == ""))
    {
        bool enable;
        if (m_ctrlFactory->getPermissionCheckController()->hasPermission(enable))
        {
            if (enable)
            {
                if (m_senseInterface->exportAdjData(QDateTime::currentDateTime()))
                    ret = ZeraMControllerIo::cmddone;
                else
                    ret = ZeraMControllerIo::cmdexecfault;
            }
            else
                ret = ZeraMControllerIo::cmdexecfault;
        }
        else ret = ZeraMControllerIo::cmdexecfault;
    }
    m_genAnswer(ret, s);
    return s;
}


QString Com5003SystemInterface::m_AdjFlashRead(QString &sInput)
{
    QString s;
    int ret = ZeraMControllerIo::cmdfault;
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1) && (cmd.getParam(0) == ""))
    {
        if (m_senseInterface->importAdjData())
            ret = ZeraMControllerIo::cmddone;
        else
            ret = ZeraMControllerIo::cmdexecfault;
    }
    m_genAnswer(ret, s);
    return s;
}

QString Com5003SystemInterface::m_AdjXmlImportExport(QString &sInput)
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
                else {
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


QString Com5003SystemInterface::m_AdjXMLWrite(QString &sInput)
{
    QString s;
    int ret = ZeraMControllerIo::cmdfault;
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1))
    {
        QString filename = cmd.getParam(0);
        if (m_senseInterface->exportAdTojXMLFile(filename))
            ret = ZeraMControllerIo::cmddone;
        else
            ret = ZeraMControllerIo::cmdexecfault;
    }
    m_genAnswer(ret, s);
    return s;
}


QString Com5003SystemInterface::m_AdjXMLRead(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isCommand(1)) {
        bool enable = false;
        m_ctrlFactory->getPermissionCheckController()->hasPermission(enable);
        if (enable) {
            QString filename = cmd.getParam(0);
            if (m_senseInterface->importAdjXMLFile(filename))
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            else
                return ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::erraut];
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Com5003SystemInterface::m_AdjFlashChksum(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        QString s = QString("0x%1").arg(m_senseInterface->getAdjChecksum(),0,16); // hex output
        return s;
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

void Com5003SystemInterface::updateAllCtrlVersionsJson()
{
    QJsonObject object;
    object.insert("Relay controller version", QJsonValue::fromVariant(m_systemInfo->getCTRLVersion()));
    QJsonDocument doc(object);
    m_allCtrlVersion = doc.toJson(QJsonDocument::Compact);
}

void Com5003SystemInterface::updateAllPCBsVersion()
{
    QJsonObject object;
    object.insert("Relay PCB version", QJsonValue::fromVariant(m_systemInfo->getPCBVersion()));
    QJsonDocument doc(object);
    m_allPCBVersion = doc.toJson(QJsonDocument::Compact);
}

void Com5003SystemInterface::m_genAnswer(int select, QString &answer)
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



