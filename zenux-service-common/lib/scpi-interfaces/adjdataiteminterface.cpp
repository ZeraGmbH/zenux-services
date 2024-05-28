#include "adjdataiteminterface.h"
#include "zscpi_response_definitions.h"
#include <gaussmatrix.h>
#include <gaussnode.h>
#include <math.h>

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
    initJustData(param.init);
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
        return QString("%1").arg(getCoefficient(index));
    else if (cmd.isCommand(1)) {
        bool enable;
        if (m_checkPermission(enable)) {
            if (enable) {
                QString spar = cmd.getParam(0);
                bool ok;
                double par = spar.toDouble(&ok);
                if (ok) {
                    setCoefficient(index, par);
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
        return QString("%1").arg(getNode(index)->toString(m_digits));
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
                    setNode(index, jn);
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


bool AdjDataItemInterface::setNode(int index, AdjustmentNode jn) // // !!! setting node sequence is relevant !!!
{
    if (index < m_adjItem->getOrder()+1) {
        for (int i = index; i < m_adjItem->getOrder()+1; i++)
            m_adjItem->m_adjNodes[i] = jn;
        return true;
    }
    return false;
}

AdjustmentNode* AdjDataItemInterface::getNode(int index) // can be read back
{
    return &m_adjItem->m_adjNodes[index];
}

bool AdjDataItemInterface::setCoefficient(int index, double value)
{
    if (index < m_adjItem->getOrder()+1) {
        m_adjItem->m_adjCoefficients[index] = value;
        if (index < m_adjItem->getOrder())
            for (int i = index+1; i < m_adjItem->getOrder()+1; i++)
                m_adjItem->m_adjCoefficients[i] = 0.0;
        return true;
    }
    return false;
}

double AdjDataItemInterface::getCoefficient(int index)
{
    return m_adjItem->m_adjCoefficients[index];
}

bool AdjDataItemInterface::calcCoefficientsFromNodes()
{
    const double epsilon = 1e-7;
    int i;
    int realOrd = 0;
    if (m_adjItem->getOrder() > 0) { // only done if noticed order > 0
        for (i = 0;i < m_adjItem->getOrder(); i++) {
            if (fabs(m_adjItem->m_adjNodes[i].getArgument() - m_adjItem->m_adjNodes[i+1].getArgument()) < epsilon)
                break;
            realOrd++;
        }
    }
    // fill the matrix
    cGaussMatrix *Matrix;
    Matrix = new cGaussMatrix(realOrd+1);
    cGaussNode gn;
    for (i = 0; i < realOrd+1; i++) {
        gn.m_fNode = m_adjItem->m_adjNodes[i].getCorrection();
        gn.m_fArg = m_adjItem->m_adjNodes[i].getArgument();
        Matrix->setMatrix(i, gn);
    }
    // matrix computation
    Matrix->cmpKoeff();
    // read out coefficients
    for (i = 0; i < realOrd+1; i++)
        setCoefficient(i, Matrix->getKoeff(realOrd-i));
    // not calculated coefficient are set to 0
    for (i = i; i < m_adjItem->getOrder()+1; i++)
    setCoefficient(i, 0.0);
    delete Matrix;
    return true;
}

quint8 AdjDataItemInterface::getStatus()
{
    return m_adjItem->m_adjStatus;
}

void AdjDataItemInterface::initJustData(double init)
{
    setNode(0, AdjustmentNode(init, 0.0)); // setting the 1st node and all following
    calcCoefficientsFromNodes();
    m_adjItem->m_adjStatus = 0;
}

double AdjDataItemInterface::getCorrection(double arg) // calculates correction value
{
    double Arg = 1.0;
    double Corr = 0.0;
    for (int i = 0; i < m_adjItem->getOrder()+1; i++) { // correction function has nth order
        Corr += m_adjItem->m_adjCoefficients[i] * Arg;
        Arg *= arg;
    }
    return Corr;
}
