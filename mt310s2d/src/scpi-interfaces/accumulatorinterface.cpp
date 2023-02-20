#include "accumulatorinterface.h"

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

}
