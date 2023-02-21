#include "accumulatorinterface.h"
#include "atmelsysctrl.h"

constexpr quint16 ERROR = 0x80;

AccumulatorInterface::AccumulatorInterface(cSCPI *scpiInterface) :
    ScpiConnection(scpiInterface)
{
}

void AccumulatorInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SYSTEM:ACCUMULATOR").arg(leadingNodes),"STATUS",SCPI::isQuery, m_pSCPIInterface, accumulatorCommands::cmdStatus);
}

void AccumulatorInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case accumulatorCommands::cmdStatus:
        protoCmd->m_sOutput = getAccumulatorStatus();
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString AccumulatorInterface::getAccumulatorStatus()
{
    quint16 status = 0;
    if(pAtmelSys->readAccumulatorStatus(status) == ZeraMcontrollerBase::atmelRM::cmddone){
        m_status = status;
    }
    else{
        m_status = ERROR;
    }
    return QString::number(m_status);
}
