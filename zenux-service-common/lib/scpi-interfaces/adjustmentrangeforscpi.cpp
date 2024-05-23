#include "adjustmentnode.h"
#include "zscpi_response_definitions.h"
#include "adjustmentrangeforscpi.h"

enum JustCommands
{
    JustStatus,
    JustCoefficient0,
    JustCoefficient1,
    JustCoefficient2,
    JustCoefficient3,
    JustNode0,
    JustNode1,
    JustNode2,
    JustNode3
};

AdjustmentRangeForSCPI::AdjustmentRangeForSCPI(cSCPI *scpi) :
    ScpiConnection(scpi)
{
    //status & function check....
}

void AdjustmentRangeForSCPI::initSCPIConnection(QString leadingNodes)
{
    addDelegate(QString("%1").arg(leadingNodes), "STATUS", SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, JustStatus);

    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes); // upper lines do not expect trailing ':' - we should use lists for leading nodes...
    addDelegate(QString("%1COEFFICIENT").arg(leadingNodes), "0", SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, JustCoefficient0);
    addDelegate(QString("%1COEFFICIENT").arg(leadingNodes), "1", SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, JustCoefficient1);
    addDelegate(QString("%1COEFFICIENT").arg(leadingNodes), "2", SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, JustCoefficient2);
    addDelegate(QString("%1COEFFICIENT").arg(leadingNodes), "3", SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, JustCoefficient3);
    addDelegate(QString("%1NODE").arg(leadingNodes), "0", SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, JustNode0);
    addDelegate(QString("%1NODE").arg(leadingNodes), "1", SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, JustNode1);
    addDelegate(QString("%1NODE").arg(leadingNodes), "2", SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, JustNode2);
    addDelegate(QString("%1NODE").arg(leadingNodes), "3", SCPI::isCmdwP | SCPI::isQuery, m_pSCPIInterface, JustNode3);
}

void AdjustmentRangeForSCPI::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case JustStatus:
        protoCmd->m_sOutput = scpiReadWriteStatus(protoCmd->m_sInput);
        break;
    case JustCoefficient0:
        protoCmd->m_sOutput = scpiReadWriteJustCoeeficient(protoCmd->m_sInput, 0);
        break;
    case JustCoefficient1:
        protoCmd->m_sOutput = scpiReadWriteJustCoeeficient(protoCmd->m_sInput, 1);
        break;
    case JustCoefficient2:
        protoCmd->m_sOutput = scpiReadWriteJustCoeeficient(protoCmd->m_sInput, 2);
        break;
    case JustCoefficient3:
        protoCmd->m_sOutput = scpiReadWriteJustCoeeficient(protoCmd->m_sInput, 3);
        break;
    case JustNode0:
        protoCmd->m_sOutput = scpiReadWriteJustNode(protoCmd->m_sInput, 0);
        break;
    case JustNode1:
        protoCmd->m_sOutput = scpiReadWriteJustNode(protoCmd->m_sInput, 1);
        break;
    case JustNode2:
        protoCmd->m_sOutput = scpiReadWriteJustNode(protoCmd->m_sInput, 2);
        break;
    case JustNode3:
        protoCmd->m_sOutput = scpiReadWriteJustNode(protoCmd->m_sInput, 3);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString AdjustmentRangeForSCPI::scpiReadWriteStatus(QString &sInput)
{
}

QString AdjustmentRangeForSCPI::scpiReadWriteJustCoeeficient(QString &sInput, quint8 index)
{

}

QString AdjustmentRangeForSCPI::scpiReadWriteJustNode(QString &sInput, quint8 index)
{

}
