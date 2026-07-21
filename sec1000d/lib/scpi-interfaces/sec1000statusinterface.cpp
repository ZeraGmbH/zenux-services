#include "sec1000statusinterface.h"
#include "zscpi_response_definitions.h"

Sec1000StatusInterface::Sec1000StatusInterface(std::shared_ptr<cSCPI> scpiInterface) :
    ScpiServerConnection(scpiInterface)
{
}

enum StatusCommands
{
    cmdDevice,
};

void Sec1000StatusInterface::initSCPIConnection()
{
    addDelegate("STATUS", "DEVICE",SCPI::isQuery, m_scpiInterface, cmdDevice);
}

void Sec1000StatusInterface::executeProtoScpi(int cmdCode, const ProtonetCommandPtr &protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;

    if (cmd.isQuery())
    {
        switch (cmdCode)
        {
        case cmdDevice:
            protoCmd->m_sOutput = QString("%1").arg(getDeviceStatus());
            break; // StatusDevice
        }
    }
    else
        protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

quint16 Sec1000StatusInterface::getDeviceStatus()
{
    return 1; // device is present per default
}
