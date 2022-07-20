#include <math.h>
#include <scpi.h>
#include <gaussmatrix.h>
#include <gaussnode.h>
#include <protonetcommand.h>
#include <scpiconnection.h>
#include <scpidelegate.h>
#include "justdata.h"
#include <justnode.h>
#include "micro-controller-io/atmel.h"

cJustData::cJustData(cSCPI* scpiinterface, int order, double init) :
    cSCPIConnection(scpiinterface),
    m_nOrder(order)
{
    m_pCoefficient = new double[order+1];  
    m_pJustNode = new cJustNode[order+1]; 
    initJustData(init);
}


cJustData::~cJustData()
{
    delete [] m_pCoefficient;
    delete [] m_pJustNode;
}


void cJustData::initSCPIConnection(QString leadingNodes)
{
    cSCPIDelegate* delegate;

    delegate = new cSCPIDelegate(QString("%1").arg(leadingNodes), "STATUS", SCPI::isCmdwP || SCPI::isQuery, m_pSCPIInterface, JustStatus);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1COEFFICIENT").arg(leadingNodes), "0", SCPI::isCmdwP || SCPI::isQuery, m_pSCPIInterface, JustCoefficient0);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1COEFFICIENT").arg(leadingNodes), "1", SCPI::isCmdwP || SCPI::isQuery, m_pSCPIInterface, JustCoefficient1);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1COEFFICIENT").arg(leadingNodes), "2", SCPI::isCmdwP || SCPI::isQuery, m_pSCPIInterface, JustCoefficient2);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1COEFFICIENT").arg(leadingNodes), "3", SCPI::isCmdwP || SCPI::isQuery, m_pSCPIInterface, JustCoefficient3);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1NODE").arg(leadingNodes), "0", SCPI::isCmdwP || SCPI::isQuery, m_pSCPIInterface, JustNode0);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1NODE").arg(leadingNodes), "1", SCPI::isCmdwP || SCPI::isQuery, m_pSCPIInterface, JustNode1);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1NODE").arg(leadingNodes), "2", SCPI::isCmdwP || SCPI::isQuery, m_pSCPIInterface, JustNode2);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1NODE").arg(leadingNodes), "3", SCPI::isCmdwP || SCPI::isQuery, m_pSCPIInterface, JustNode3);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));

}


void cJustData::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
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


QString cJustData::m_ReadWriteStatus(QString &sInput)
{
    bool ok;
    bool enable;

    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        return QString("%1").arg(m_nStatus);
    }
    else
    {
        if (cmd.isCommand(1))
        {
            if (pAtmel->getEEPROMAccessEnable(enable) == ZeraMcontrollerBase::cmddone)
            {
                if (enable)
                {
                    QString spar = cmd.getParam(0);
                    quint8 par = spar.toInt(&ok);
                    if (ok)
                    {
                        m_nStatus = par;
                        return SCPI::scpiAnswer[SCPI::ack];
                    }
                    else
                        return SCPI::scpiAnswer[SCPI::errval];
                }
                else
                    return SCPI::scpiAnswer[SCPI::erraut];
            }
            else return SCPI::scpiAnswer[SCPI::errexec];
        }
    }

    return SCPI::scpiAnswer[SCPI::nak];
}


QString cJustData::m_ReadWriteJustCoeeficient(QString &sInput, quint8 index)
{
    bool ok;

    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        return QString("%1").arg(getCoefficient(index));
    }
    else
    {
        if (cmd.isCommand(1))
        {
            bool enable;
            if (pAtmel->getEEPROMAccessEnable(enable) == ZeraMcontrollerBase::cmddone)
            {
                if (enable)
                {
                    QString spar = cmd.getParam(0);
                    double par = spar.toDouble(&ok);
                    if (ok)
                    {
                        setCoefficient(index, par);
                        return SCPI::scpiAnswer[SCPI::ack];
                    }
                    else
                        return SCPI::scpiAnswer[SCPI::errval];
                }
                else
                    return SCPI::scpiAnswer[SCPI::erraut];
            }
            else
                return SCPI::scpiAnswer[SCPI::errexec];
        }
    }

    return SCPI::scpiAnswer[SCPI::nak];
}


QString cJustData::m_ReadWriteJustNode(QString &sInput, quint8 index)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        return QString("%1").arg(getNode(index)->Serialize(6));
    }
    else
    {
        if (cmd.isCommand(2))
        {
            bool enable;
            bool ok0, ok1;
            if (pAtmel->getEEPROMAccessEnable(enable) == ZeraMcontrollerBase::cmddone)
            {
                if (enable)
                {
                    QString spar = cmd.getParam(0);
                    double par0 = spar.toDouble(&ok0);
                    spar = cmd.getParam(1);
                    double par1 = spar.toDouble(&ok1);
                    if (ok0 && ok1)
                    {
                        cJustNode jn = cJustNode(par0,par1);
                        setNode(index, jn);
                        return SCPI::scpiAnswer[SCPI::ack];
                    }
                    else
                        return SCPI::scpiAnswer[SCPI::errval];
                }
                else
                    return SCPI::scpiAnswer[SCPI::erraut];
            }
            else
                return SCPI::scpiAnswer[SCPI::errexec];
        }
    }

    return SCPI::scpiAnswer[SCPI::nak];
}


void cJustData::Serialize(QDataStream& qds) // writes adjustment data to a qdatastream
{
    int i;
    qds << m_nStatus;
    for (i = 0; i < m_nOrder+1; i++)
        qds << m_pCoefficient[i];
    for (i = 0; i < m_nOrder+1; i++)
        m_pJustNode[i].Serialize(qds);
}


void cJustData::Deserialize(QDataStream& qds) // reads adjustment data from a qdatastream
{
    int i;
    qds >> m_nStatus;
    for (i = 0; i < m_nOrder+1; i++)
        qds >> m_pCoefficient[i];
    for (i = 0; i < m_nOrder+1; i++)
        m_pJustNode[i].Deserialize(qds);
}


QString cJustData::SerializeStatus()
{
    QString s = QString("%1").arg(m_nStatus);
    return s;
}
	
	
QString cJustData::SerializeCoefficients() // writes adjustment data to qstring
{
    int i;
    QString s = "";
    for (i = 0; i < m_nOrder+1; i++)
        s += QString("%1;").arg(m_pCoefficient[i],0,'f',12);
    return s;
}


QString cJustData::SerializeNodes()
{
    int i;
    QString s = "";
    for (i = 0; i < m_nOrder+1; i++)
        s += m_pJustNode[i].Serialize(6);
    return s;
}


void cJustData::DeserializeStatus(const QString &s)
{
    bool ok;
    m_nStatus = s.toInt(&ok);
}


void cJustData::DeserializeCoefficients(const QString& s) 
{	
    int i;
    for (i = 0; i < m_nOrder+1; i++)
        m_pCoefficient[i] = s.section(';',i,i).toDouble();
}


void cJustData::DeserializeNodes(const QString& s)
{
    int i;
    for (i = 0; i < m_nOrder+1; i++)
        m_pJustNode[i].Deserialize(s.section(';',i << 1,(i << 1) + 1));
}


bool cJustData::setNode(int index, cJustNode jn) // // !!! setting node sequence is relevant !!!
{
    if (index < m_nOrder+1)
    {
        for (int i = index; i < m_nOrder+1; i++)
            m_pJustNode[i] = jn;
        return true;
    }
    return false;
}


cJustNode* cJustData::getNode(int index) // can be read back
{
    return &m_pJustNode[index];
}
 

bool cJustData::setCoefficient(int index, double value)
{
    if (index < m_nOrder+1)
    {
        int i;
        m_pCoefficient[index] = value;
        if (index < m_nOrder)
            for (i = index+1; i < m_nOrder+1; i++)
                m_pCoefficient[i] = 0.0;
        return true;
    }
    return false;
}


double cJustData::getCoefficient(int index)
{
    return m_pCoefficient[index];
}


bool cJustData::cmpCoefficients() // calculates coefficients from nodes
{
    const double epsilon = 1e-7;
    int realOrd, i; // find out real
    
    realOrd = 0;
    if (m_nOrder > 0) // only done if noticed order > 0
    {
        for (i = 0;i < m_nOrder; i++)
        {
            if (fabs(m_pJustNode[i].getArgument() - m_pJustNode[i+1].getArgument()) < epsilon)
                break;
            realOrd++;
        }
    }
    
    // fill the matrix
    cGaussMatrix *Matrix;
    Matrix = new cGaussMatrix(realOrd+1);
    cGaussNode gn;
    for (i = 0; i < realOrd+1; i++)
    {
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


quint8 cJustData::getStatus()
{
    return m_nStatus;
}


void cJustData::initJustData(double init)
{
    setNode(0 , cJustNode(init,0.0)); // setting the 1st node and all following
    cmpCoefficients();
    m_nStatus = 0;
}

    
double cJustData::getCorrection(double arg) // calculates correction value
{
    double Arg = 1.0;
    double Corr = 0.0;
    for (int i = 0; i < m_nOrder+1; i++) // correction function has nth order
    {
        Corr += m_pCoefficient[i] * Arg;
        Arg *= arg;
    }
    
    return Corr;
}    
    


