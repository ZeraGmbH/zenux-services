#include "statusinterface.h"
#include "sec1000dglobal.h"
#include "protonetcommand.h"
#include "scpisingletonfactory.h"

cStatusInterface::cStatusInterface() :
    ScpiConnection(ScpiSingletonFactory::getScpiObj())
{
}

void cStatusInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1STATUS").arg(leadingNodes),"DEVICE",SCPI::isQuery, m_pSCPIInterface, StatusSystem::cmdDevice);
}


void cStatusInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
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
        protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


quint16 cStatusInterface::getDeviceStatus()
{
    return 1; // device is present per default
}





