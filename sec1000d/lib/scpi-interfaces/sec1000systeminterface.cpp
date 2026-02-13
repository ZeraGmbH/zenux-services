#include "sec1000systeminterface.h"
#include "sec1000d.h"
#include "sec1000systeminfo.h"
#include "commonscpimethods.h"
#include "zscpi_response_definitions.h"
#include "protonetcommand.h"
#include <scpi.h>
#include <scpicommand.h>

cSystemInterface::cSystemInterface(std::shared_ptr<cSCPI> scpiInterface, cSEC1000dServer *server, Sec1000SystemInfo *sInfo) :
    ScpiServerConnection(scpiInterface),
    m_pMyServer(server),
    m_pSystemInfo(sInfo)
{
}

void cSystemInterface::initSCPIConnection()
{
    addDelegate("SYSTEM:VERSION", "SERVER", SCPI::isQuery, m_scpiInterface, SystemSystem::cmdVersionServer);
    addDelegate("SYSTEM:VERSION", "DEVICE", SCPI::isQuery, m_scpiInterface, SystemSystem::cmdVersionDevice);
    addDelegate("SYSTEM:VERSION", "PCB", SCPI::isQuery, m_scpiInterface, SystemSystem::cmdVersionPCB);
    addDelegate("SYSTEM:VERSION", "FPGA", SCPI::isQuery, m_scpiInterface, SystemSystem::cmdVersionFPGA);
    addDelegate("SYSTEM", "SERIAL", SCPI::isQuery | SCPI::isCmdwP , m_scpiInterface, SystemSystem::cmdSerialNumber);
    addDelegate("SYSTEM:INTERFACE", "READ", SCPI::isQuery, m_scpiInterface, SystemSystem::cmdInterfaceRead);
}

void cSystemInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
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
    case SystemSystem::cmdVersionFPGA:
        protoCmd->m_sOutput = m_ReadFPGAVersion(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdSerialNumber:
        protoCmd->m_sOutput = m_ReadWriteSerialNumber(protoCmd->m_sInput);
        break;
    case SystemSystem::cmdInterfaceRead:
        protoCmd->m_sOutput = CommonScpiMethods::handleScpiInterfaceRead(m_scpiInterface, protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


QString cSystemInterface::scpiReadServerVersion(QString &sInput)
{
    QString s;
    cSCPICommand cmd = sInput;
    if ( cmd.isQuery() )
        s = m_pMyServer->getVersion();
    else
        s = ZSCPI::scpiAnswer[ZSCPI::nak];
    return s;
}


QString cSystemInterface::m_ReadDeviceVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        if (m_pSystemInfo->dataRead())
            return m_pSystemInfo->getDeviceVersion();
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString cSystemInterface::m_ReadDeviceName(QString& sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        if (m_pSystemInfo->dataRead())
            return m_pSystemInfo->getDeviceName();
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString cSystemInterface::scpiReadPCBVersion(QString &sInput)
{
    QString s;
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        if (m_pSystemInfo->dataRead())
            s = m_pSystemInfo->getPCBVersion();
        else
            s = ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        s = ZSCPI::scpiAnswer[ZSCPI::nak];
    return s;
}

QString cSystemInterface::m_ReadFPGAVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        if (m_pSystemInfo->dataRead())
            return m_pSystemInfo->getLCAVersion();
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString cSystemInterface::m_ReadWriteSerialNumber(QString &sInput)
{
    QString s;
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        if (m_pSystemInfo->dataRead())
            s = m_pSystemInfo->getSerialNumber();
        else
            s = ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else {
        if (cmd.isCommand(1)) {
            //QString Serial = cmd.getParam(0);
            // todo write the serial number
            m_pSystemInfo->getSystemInfo(); // read back info
            s = ZSCPI::scpiAnswer[ZSCPI::ack];
        }
        else
            s = ZSCPI::scpiAnswer[ZSCPI::nak];
    }
    return s;
}
