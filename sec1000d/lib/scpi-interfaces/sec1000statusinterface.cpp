#include "sec1000statusinterface.h"
#include "zscpi_response_definitions.h"
#include "protonetcommand.h"

Sec1000StatusInterface::Sec1000StatusInterface(std::shared_ptr<cSCPI> scpiInterface) :
    ScpiConnection(scpiInterface)
{
}

void Sec1000StatusInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1STATUS").arg(leadingNodes),"DEVICE",SCPI::isQuery, m_scpiInterface, StatusSystem::cmdDevice);
}

void Sec1000StatusInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;

    if (cmd.isQuery())
    {
        switch (cmdCode)
        {
        case StatusSystem::cmdDevice:
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
