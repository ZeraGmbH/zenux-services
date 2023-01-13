#include "statusinterface.h"
#include "protonetcommand.h"


cStatusInterface::cStatusInterface() :
    ScpiConnection(ScpiSingletonFactory::getScpiObj(ServerName))
{
}

void cStatusInterface::initSCPIConnection(QString leadingNodes)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1STATUS").arg(leadingNodes),"DEVICE",SCPI::isQuery, m_pSCPIInterface, StatusSystem::cmdDevice);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &cStatusInterface::executeCommand);
}


void cStatusInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
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





