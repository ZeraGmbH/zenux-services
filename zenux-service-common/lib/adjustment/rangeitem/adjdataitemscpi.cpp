#include "adjdataitemscpi.h"
#include "zscpi_response_definitions.h"

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

AdjDataItemScpi::AdjDataItemScpi(TJustDataParam param, AdjDataItem *adjItem) :
    ScpiConnection(param.scpiinterface),
    m_checkPermission(param.checkPermission),
    m_adjItem(adjItem),
    m_digits(param.digits)
{
}

void AdjDataItemScpi::initSCPIConnection(QString leadingNodes)
{
    addDelegate(QString("%1").arg(leadingNodes), "STATUS", SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, JustStatus);

    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes); // upper lines do not expect trailing ':' - we should use lists for leading nodes...
    addDelegate(QString("%1COEFFICIENT").arg(leadingNodes), "0", SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, JustCoefficient0);
    addDelegate(QString("%1COEFFICIENT").arg(leadingNodes), "1", SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, JustCoefficient1);
    addDelegate(QString("%1COEFFICIENT").arg(leadingNodes), "2", SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, JustCoefficient2);
    addDelegate(QString("%1COEFFICIENT").arg(leadingNodes), "3", SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, JustCoefficient3);
    addDelegate(QString("%1NODE").arg(leadingNodes), "0", SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, JustNode0);
    addDelegate(QString("%1NODE").arg(leadingNodes), "1", SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, JustNode1);
    addDelegate(QString("%1NODE").arg(leadingNodes), "2", SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, JustNode2);
    addDelegate(QString("%1NODE").arg(leadingNodes), "3", SCPI::isCmdwP | SCPI::isQuery, m_scpiInterface, JustNode3);
}

AdjDataItem *AdjDataItemScpi::getAdjItem()
{
    return m_adjItem;
}

void AdjDataItemScpi::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
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

QString AdjDataItemScpi::scpiReadWriteStatus(const QString &scpi)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(m_adjItem->getAdjStatus());
    else if (cmd.isCommand(1)) {
        bool enable;
        if (m_checkPermission(enable)) {
            if (enable) {
                QString spar = cmd.getParam(0);
                bool ok;
                quint8 par = spar.toInt(&ok);
                if (ok) {
                    m_adjItem->setAdjStatus(par);
                    return ZSCPI::scpiAnswer[ZSCPI::ack];
                }
                else
                    return ZSCPI::scpiAnswer[ZSCPI::errval];
            }
            else
                return ZSCPI::scpiAnswer[ZSCPI::erraut];
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString AdjDataItemScpi::scpiReadWriteJustCoeeficient(const QString &scpi, quint8 index)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(getAdjItem()->getCoefficient(index));
    else if (cmd.isCommand(1)) {
        bool enable;
        if (m_checkPermission(enable)) {
            if (enable) {
                QString spar = cmd.getParam(0);
                bool ok;
                double par = spar.toDouble(&ok);
                if (ok) {
                    getAdjItem()->setCoefficient(index, par);
                    return ZSCPI::scpiAnswer[ZSCPI::ack];
                }
                else
                    return ZSCPI::scpiAnswer[ZSCPI::errval];
            }
            else
                return ZSCPI::scpiAnswer[ZSCPI::erraut];
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}


QString AdjDataItemScpi::scpiReadWriteJustNode(const QString &scpi, quint8 index)
{
    cSCPICommand cmd = scpi;
    if (cmd.isQuery())
        return QString("%1").arg(getAdjItem()->getNode(index).toString(m_digits));
    else if (cmd.isCommand(2))
    {
        bool enable;
        if (m_checkPermission(enable)) {
            if (enable) {
                QString spar = cmd.getParam(0);
                bool ok0;
                double par0 = spar.toDouble(&ok0);
                spar = cmd.getParam(1);
                bool ok1;
                double par1 = spar.toDouble(&ok1);
                if (ok0 && ok1)
                {
                    AdjDataNode node = AdjDataNode(par0,par1);
                    getAdjItem()->setNode(index, node);
                    return ZSCPI::scpiAnswer[ZSCPI::ack];
                }
                else
                    return ZSCPI::scpiAnswer[ZSCPI::errval];
            }
            else
                return ZSCPI::scpiAnswer[ZSCPI::erraut];
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    return ZSCPI::scpiAnswer[ZSCPI::nak];
}

void AdjDataItemScpi::statusFromString(const QString &s)
{
    m_adjItem->setAdjStatus(s.toInt());
}

QString AdjDataItemScpi::statusToString()
{
    return QString("%1").arg(m_adjItem->getAdjStatus());
}

void AdjDataItemScpi::nodesFromString(const QString& s)
{
    m_adjItem->nodesFromString(s);
}

QString AdjDataItemScpi::nodesToString() const
{
    return m_adjItem->nodesToString(m_digits);
}

void AdjDataItemScpi::coefficientsFromString(const QString& s)
{
    m_adjItem->coefficientsFromString(s);
}

QString AdjDataItemScpi::coefficientsToString() const
{
    return m_adjItem->coefficientsToString();
}

quint8 AdjDataItemScpi::getStatus() const
{
    return m_adjItem->getAdjStatus();
}
