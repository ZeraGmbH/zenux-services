#include "accumulatorinterface.h"

constexpr quint8 ERROR = 0x80;

AccumulatorInterface::AccumulatorInterface(cSCPI *scpiInterface, cATMELSysCtrl *atmelSysCntrl) :
    ScpiConnection(scpiInterface),
    m_atmelSysCntrl(atmelSysCntrl)
{
}

void AccumulatorInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SYSTEM:ACCUMULATOR").arg(leadingNodes),"STATUS",SCPI::isQuery, m_pSCPIInterface, accumulatorCommands::cmdStatus, &m_accumulatorStatus);
    addDelegate(QString("%1SYSTEM:ACCUMULATOR").arg(leadingNodes),"SOC",SCPI::isQuery, m_pSCPIInterface, accumulatorCommands::cmdSoc, &m_accumulatorSoc);
}

void AccumulatorInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case accumulatorCommands::cmdStatus:
        protoCmd->m_sOutput = getAccumulatorStatus();
        break;
    case accumulatorCommands::cmdSoc:
        protoCmd->m_sOutput = getAccumulatorSoc();
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString AccumulatorInterface::getAccumulatorStatus()
{
    quint8 status = 0;
    if(m_atmelSysCntrl->readAccumulatorStatus(status) == ZeraMcontrollerBase::atmelRM::cmddone){
        m_accumulatorStatus = QString::number(status);
    }
    else{
        m_accumulatorStatus = QString::number(ERROR);
    }
    return m_accumulatorStatus.getString();
}

QString AccumulatorInterface::getAccumulatorSoc()
{
    quint8 charge = 0;
    if(m_atmelSysCntrl->readAccumulatorSoc(charge) == ZeraMcontrollerBase::atmelRM::cmddone){
        m_accumulatorSoc = QString::number(charge);
    }
    else{
        m_accumulatorSoc = QString::number(ERROR);
    }
    return m_accumulatorSoc.getString();
}
