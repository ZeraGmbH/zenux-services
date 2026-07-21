#include "sec1000systeminterface.h"
#include "sec1000d.h"
#include "sec1000systeminfo.h"
#include "commonscpimethods.h"
#include "zscpi_response_definitions.h"
#include "protonetcommand.h"
#include <scpi.h>
#include <scpicommand.h>

Sec1000SystemInterface::Sec1000SystemInterface(std::shared_ptr<cSCPI> scpiInterface, cSEC1000dServer *server, Sec1000SystemInfo *sInfo) :
    ScpiServerConnection(scpiInterface),
    m_pMyServer(server),
    m_pSystemInfo(sInfo)
{
}

enum SystemCommands
{
    cmdVersionServer,
    cmdVersionDevice,
    cmdVersionPCB,
    cmdVersionFPGA,
    cmdSerialNumber,
    cmdInterfaceRead
};

void Sec1000SystemInterface::initSCPIConnection()
{
    addDelegate("SYSTEM:VERSION", "SERVER", SCPI::isQuery, m_scpiInterface, cmdVersionServer);
    addDelegate("SYSTEM:VERSION", "DEVICE", SCPI::isQuery, m_scpiInterface, cmdVersionDevice);
    addDelegate("SYSTEM:VERSION", "PCB", SCPI::isQuery, m_scpiInterface, cmdVersionPCB);
    addDelegate("SYSTEM:VERSION", "FPGA", SCPI::isQuery, m_scpiInterface, cmdVersionFPGA);
    addDelegate("SYSTEM", "SERIAL", SCPI::isQuery | SCPI::isCmdwP , m_scpiInterface, cmdSerialNumber);
    addDelegate("SYSTEM:INTERFACE", "READ", SCPI::isQuery, m_scpiInterface, cmdInterfaceRead);
}

void Sec1000SystemInterface::executeProtoScpi(int cmdCode, const ProtonetCommandPtr &protoCmd)
{
    switch (cmdCode)
    {
    case cmdVersionServer:
        protoCmd->m_sOutput = scpiReadServerVersion(protoCmd->m_sInput);
        break;
    case cmdVersionDevice:
        protoCmd->m_sOutput = scpiReadDeviceVersion(protoCmd->m_sInput);
        break;
    case cmdVersionPCB:
        protoCmd->m_sOutput = scpiReadPCBVersion(protoCmd->m_sInput);
        break;
    case cmdVersionFPGA:
        protoCmd->m_sOutput = scpiReadFPGAVersion(protoCmd->m_sInput);
        break;
    case cmdSerialNumber:
        protoCmd->m_sOutput = scpiReadWriteSerialNumber(protoCmd->m_sInput);
        break;
    case cmdInterfaceRead:
        protoCmd->m_sOutput = CommonScpiMethods::handleScpiInterfaceRead(m_scpiInterface, protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


QString Sec1000SystemInterface::scpiReadServerVersion(const QString &scpi)
{
    QString s;
    cSCPICommand cmd = scpi;
    if ( cmd.isQuery() )
        s = m_pMyServer->getVersion();
    else
        s = ZSCPI::scpiAnswer[ZSCPI::nak];
    return s;
}


QString Sec1000SystemInterface::scpiReadDeviceVersion(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery()) {
        if (m_pSystemInfo->dataRead())
            return m_pSystemInfo->getDeviceVersion();
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Sec1000SystemInterface::scpiReadDeviceName(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery()) {
        if (m_pSystemInfo->dataRead())
            return m_pSystemInfo->getDeviceName();
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString Sec1000SystemInterface::scpiReadPCBVersion(const QString &scpi)
{
    QString s;
    cSCPICommand cmd = scpi;
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

QString Sec1000SystemInterface::scpiReadFPGAVersion(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery()) {
        if (m_pSystemInfo->dataRead())
            return m_pSystemInfo->getLCAVersion();
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Sec1000SystemInterface::scpiReadWriteSerialNumber(const QString &scpi)
{
    QString s;
    cSCPICommand cmd = scpi;
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
