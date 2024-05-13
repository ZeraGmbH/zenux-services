#include "justdatainterface.h"
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

JustDataInterface::JustDataInterface(TJustDataParam param) :
    ScpiConnection(param.scpiinterface),
    m_checkPermission(param.checkPermission),
    m_nOrder(param.order),
    m_digits(param.digits)
{
    m_pCoefficient = new double[param.order+1];
    m_pJustNode = new AdjustmentNode[param.order+1];
    initJustData(param.init);
}

JustDataInterface::~JustDataInterface()
{
    delete [] m_pCoefficient;
    delete [] m_pJustNode;
}

void JustDataInterface::initSCPIConnection(QString leadingNodes)
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

void JustDataInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case JustStatus:
        protoCmd->m_sOutput = m_ReadWriteStatus(protoCmd->m_sInput);
        break;
    case JustCoefficient0:
        protoCmd->m_sOutput = m_ReadWriteJustCoeeficient(protoCmd->m_sInput, 0);
        break;
    case JustCoefficient1:
        protoCmd->m_sOutput = m_ReadWriteJustCoeeficient(protoCmd->m_sInput, 1);
        break;
    case JustCoefficient2:
        protoCmd->m_sOutput = m_ReadWriteJustCoeeficient(protoCmd->m_sInput, 2);
        break;
    case JustCoefficient3:
        protoCmd->m_sOutput = m_ReadWriteJustCoeeficient(protoCmd->m_sInput, 3);
        break;
    case JustNode0:
        protoCmd->m_sOutput = m_ReadWriteJustNode(protoCmd->m_sInput, 0);
        break;
    case JustNode1:
        protoCmd->m_sOutput = m_ReadWriteJustNode(protoCmd->m_sInput, 1);
        break;
    case JustNode2:
        protoCmd->m_sOutput = m_ReadWriteJustNode(protoCmd->m_sInput, 2);
        break;
    case JustNode3:
        protoCmd->m_sOutput = m_ReadWriteJustNode(protoCmd->m_sInput, 3);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString JustDataInterface::m_ReadWriteStatus(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return QString("%1").arg(m_nStatus);
    else if (cmd.isCommand(1)) {
        bool enable;
        if (m_checkPermission(enable)) {
            if (enable) {
                QString spar = cmd.getParam(0);
                bool ok;
                quint8 par = spar.toInt(&ok);
                if (ok) {
                    m_nStatus = par;
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

QString JustDataInterface::m_ReadWriteJustCoeeficient(QString &sInput, quint8 index)
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


QString JustDataInterface::m_ReadWriteJustNode(QString &sInput, quint8 index)
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


void JustDataInterface::Serialize(QDataStream& qds) // writes adjustment data to a qdatastream
{
    qds << m_nStatus;
    int i;
    for (i = 0; i < m_nOrder+1; i++)
        qds << m_pCoefficient[i];
    for (i = 0; i < m_nOrder+1; i++)
        m_pJustNode[i].Serialize(qds);
}

void JustDataInterface::Deserialize(QDataStream& qds) // reads adjustment data from a qdatastream
{
    qds >> m_nStatus;
    int i;
    for (i = 0; i < m_nOrder+1; i++)
        qds >> m_pCoefficient[i];
    for (i = 0; i < m_nOrder+1; i++)
        m_pJustNode[i].Deserialize(qds);
}

QString JustDataInterface::SerializeStatus()
{
    return QString("%1").arg(m_nStatus);
}

QString JustDataInterface::SerializeCoefficients() // writes adjustment data to qstring
{
    QString s;
    for (int i = 0; i < m_nOrder+1; i++)
        s += QString("%1;").arg(m_pCoefficient[i],0,'f',12);
    return s;
}

QString JustDataInterface::SerializeNodes()
{
    QString s;
    for (int i = 0; i < m_nOrder+1; i++)
        s += m_pJustNode[i].toString(m_digits);
    return s;
}

void JustDataInterface::DeserializeStatus(const QString &s)
{
    m_nStatus = s.toInt();
}


void JustDataInterface::DeserializeCoefficients(const QString& s) 
{
    for (int i = 0; i < m_nOrder+1; i++)
        m_pCoefficient[i] = s.section(';',i,i).toDouble();
}


void JustDataInterface::DeserializeNodes(const QString& s)
{
    for (int i = 0; i < m_nOrder+1; i++)
        m_pJustNode[i].fromString(s.section(';',i << 1,(i << 1) + 1));
}


bool JustDataInterface::setNode(int index, AdjustmentNode jn) // // !!! setting node sequence is relevant !!!
{
    if (index < m_nOrder+1) {
        for (int i = index; i < m_nOrder+1; i++)
            m_pJustNode[i] = jn;
        return true;
    }
    return false;
}

AdjustmentNode* JustDataInterface::getNode(int index) // can be read back
{
    return &m_pJustNode[index];
}
 
bool JustDataInterface::setCoefficient(int index, double value)
{
    if (index < m_nOrder+1) {
        m_pCoefficient[index] = value;
        if (index < m_nOrder)
            for (int i = index+1; i < m_nOrder+1; i++)
                m_pCoefficient[i] = 0.0;
        return true;
    }
    return false;
}

double JustDataInterface::getCoefficient(int index)
{
    return m_pCoefficient[index];
}

bool JustDataInterface::cmpCoefficients() // calculates coefficients from nodes
{
    const double epsilon = 1e-7;
    int i;
    int realOrd = 0;
    if (m_nOrder > 0) { // only done if noticed order > 0
        for (i = 0;i < m_nOrder; i++) {
            if (fabs(m_pJustNode[i].getArgument() - m_pJustNode[i+1].getArgument()) < epsilon)
                break;
            realOrd++;
        }
    }
    // fill the matrix
    cGaussMatrix *Matrix;
    Matrix = new cGaussMatrix(realOrd+1);
    cGaussNode gn;
    for (i = 0; i < realOrd+1; i++) {
        gn.m_fNode = m_pJustNode[i].getCorrection();
        gn.m_fArg = m_pJustNode[i].getArgument();
        Matrix->setMatrix(i, gn);
    }
    // matrix computation
    Matrix->cmpKoeff();
    // read out coefficients
    for (i = 0; i < realOrd+1; i++)
        setCoefficient(i, Matrix->getKoeff(realOrd-i));
    // not calculated coefficient are set to 0
    for (i = i; i < m_nOrder+1; i++)
    setCoefficient(i, 0.0);
    delete Matrix;
    return true;
}

quint8 JustDataInterface::getStatus()
{
    return m_nStatus;
}

void JustDataInterface::initJustData(double init)
{
    setNode(0 , AdjustmentNode(init,0.0)); // setting the 1st node and all following
    cmpCoefficients();
    m_nStatus = 0;
}

double JustDataInterface::getCorrection(double arg) // calculates correction value
{
    double Arg = 1.0;
    double Corr = 0.0;
    for (int i = 0; i < m_nOrder+1; i++) { // correction function has nth order
        Corr += m_pCoefficient[i] * Arg;
        Arg *= arg;
    }
    return Corr;
}
