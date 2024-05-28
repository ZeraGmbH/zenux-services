#include "adjdataiteminterface.h"
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

AdjDataItemInterface::AdjDataItemInterface(TJustDataParam param, AdjDataItem *adjItem) :
    ScpiConnection(param.scpiinterface),
    m_checkPermission(param.checkPermission),
    m_adjItem(adjItem),
    m_digits(param.digits)
{
    getAdjItem()->initJustData(param.init);
}

AdjDataItemInterface::~AdjDataItemInterface()
{
}

void AdjDataItemInterface::initSCPIConnection(QString leadingNodes)
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

AdjDataItem *AdjDataItemInterface::getAdjItem()
{
    return m_adjItem;
}

void AdjDataItemInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
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

QString AdjDataItemInterface::scpiReadWriteStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("%1").arg(m_adjItem->m_adjStatus);
    else if (cmd.isCommand(1)) {
        bool enable;
        if (m_checkPermission(enable)) {
            if (enable) {
                QString spar = cmd.getParam(0);
                bool ok;
                quint8 par = spar.toInt(&ok);
                if (ok) {
                    m_adjItem->m_adjStatus = par;
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

QString AdjDataItemInterface::scpiReadWriteJustCoeeficient(QString &sInput, quint8 index)
{
    cSCPICommand cmd = sInput;
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


QString AdjDataItemInterface::scpiReadWriteJustNode(QString &sInput, quint8 index)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("%1").arg(getAdjItem()->getNode(index)->toString(m_digits));
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
                    AdjustmentNode jn = AdjustmentNode(par0,par1);
                    getAdjItem()->setNode(index, jn);
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


QString AdjDataItemInterface::statusToString()
{
    return QString("%1").arg(m_adjItem->m_adjStatus);
}

QString AdjDataItemInterface::coefficientsToString() // writes adjustment data to qstring
{
    QString s;
    for (int i = 0; i < m_adjItem->getOrder()+1; i++)
        s += QString("%1;").arg(m_adjItem->m_adjCoefficients[i],0,'f',12);
    return s;
}

QString AdjDataItemInterface::nodesToString()
{
    QString s;
    for (int i = 0; i < m_adjItem->getOrder()+1; i++)
        s += m_adjItem->m_adjNodes[i].toString(m_digits);
    return s;
}

void AdjDataItemInterface::statusFromString(const QString &s)
{
    m_adjItem->m_adjStatus = s.toInt();
}


void AdjDataItemInterface::coefficientsFromString(const QString& s)
{
    for (int i = 0; i < m_adjItem->getOrder()+1; i++)
        m_adjItem->m_adjCoefficients[i] = s.section(';',i,i).toDouble();
}


void AdjDataItemInterface::nodesFromString(const QString& s)
{
    for (int i = 0; i < m_adjItem->getOrder()+1; i++)
        m_adjItem->m_adjNodes[i].fromString(s.section(';',i << 1,(i << 1) + 1));
}


quint8 AdjDataItemInterface::getStatus()
{
    return m_adjItem->m_adjStatus;
}
