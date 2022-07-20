// implemention cCOM5003JustData

#include <qdatastream.h>
#include <scpi.h>

#include <protonetcommand.h>
#include "justdata.h"
#include <scpidelegate.h>
#include "com5003justdata.h"
#include "micro-controller-io/atmel.h"

extern cATMEL* pAtmel;

cCOM5003JustData::cCOM5003JustData(cSCPI *scpiinterface) :
    cSCPIConnection(scpiinterface)
{
    m_pGainCorrection = new cJustData(m_pSCPIInterface, GainCorrOrder, 1.0);
    m_pPhaseCorrection = new cJustData(m_pSCPIInterface, PhaseCorrOrder, 0.0);
    m_pOffsetCorrection =  new cJustData(m_pSCPIInterface, OffsetCorrOrder, 0.0);
}


cCOM5003JustData::~cCOM5003JustData()
{
    delete m_pGainCorrection; 
    delete m_pPhaseCorrection;
    delete m_pOffsetCorrection;
}


void cCOM5003JustData::initSCPIConnection(QString leadingNodes)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "GAIN", SCPI::CmdwP , m_pSCPIInterface, com5003Gain);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "ADJGAIN", SCPI::CmdwP , m_pSCPIInterface, com5003JustGain);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "PHASE", SCPI::isCmdwP, m_pSCPIInterface, com5003Phase);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "ADJPHASE", SCPI::isCmdwP, m_pSCPIInterface, com5003JustPhase);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "OFFSET", SCPI::isCmdwP, m_pSCPIInterface, com5003Offset);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "ADJOFFSET", SCPI::isCmdwP, m_pSCPIInterface, com5003JustOffset);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "STATUS", SCPI::isQuery, m_pSCPIInterface, com5003JustStatus);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "COMPUTE", SCPI::isCmdwP || SCPI::isQuery, m_pSCPIInterface, com5003JustCompute);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "INIT", SCPI::isCmdwP || SCPI::isQuery, m_pSCPIInterface, com5003JustInit);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));

    connect(m_pGainCorrection, SIGNAL(cmdExecutionDone(cProtonetCommand*)), this, SIGNAL(cmdExecutionDone(cProtonetCommand*)));
    m_pGainCorrection->initSCPIConnection(QString("%1CORRECTION:GAIN").arg(leadingNodes));
    connect(m_pPhaseCorrection, SIGNAL(cmdExecutionDone(cProtonetCommand*)), this, SIGNAL(cmdExecutionDone(cProtonetCommand*)));
    m_pPhaseCorrection->initSCPIConnection(QString("%1CORRECTION:PHASE").arg(leadingNodes));
    connect(m_pOffsetCorrection, SIGNAL(cmdExecutionDone(cProtonetCommand*)), this, SIGNAL(cmdExecutionDone(cProtonetCommand*)));
    m_pOffsetCorrection->initSCPIConnection(QString("%1CORRECTION:OFFSET").arg(leadingNodes));
}
	    

void cCOM5003JustData::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case com5003Gain:
    case com5003JustGain:
        protoCmd->m_sOutput = mReadGainCorrection(protoCmd->m_sInput);
        break;
    case com5003Phase:
    case com5003JustPhase:
        protoCmd->m_sOutput = mReadPhaseCorrection(protoCmd->m_sInput);
        break;
    case com5003Offset:
    case com5003JustOffset:
        protoCmd->m_sOutput = mReadOffsetCorrection(protoCmd->m_sInput);
        break;
    case com5003JustStatus:
        protoCmd->m_sOutput = m_ReadStatus(protoCmd->m_sInput);
        break;
    case com5003JustCompute:
        protoCmd->m_sOutput = m_ComputeJustData(protoCmd->m_sInput);
        break;
    case com5003JustInit:
        protoCmd->m_sOutput = m_InitJustData(protoCmd->m_sInput);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


QString cCOM5003JustData::mReadGainCorrection(QString& sInput)
{
    bool ok;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery(1))
    {
        QString spar = cmd.getParam(0);
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(m_pGainCorrection->getCorrection(par),0,'f',8);
        else
            return SCPI::scpiAnswer[SCPI::errval];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cCOM5003JustData::mReadPhaseCorrection(QString& sInput)
{
    bool ok;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery(1))
    {
        QString spar = cmd.getParam(0);
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(m_pPhaseCorrection->getCorrection(par),0,'f',7);
        else
            return SCPI::scpiAnswer[SCPI::errval];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];

}


QString cCOM5003JustData::mReadOffsetCorrection(QString& sInput)
{
    bool ok;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery(1))
    {
        QString spar = cmd.getParam(0);
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(m_pOffsetCorrection->getCorrection(par),0,'f',8);
        else
            return SCPI::scpiAnswer[SCPI::errval];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];

}


QString cCOM5003JustData::m_ReadStatus(QString& sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        return QString("%1").arg(getAdjustmentStatus());
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cCOM5003JustData::m_ComputeJustData(QString& sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1) && (cmd.getParam(0) == ""))
    {
        bool enable;
        if (pAtmel->getEEPROMAccessEnable(enable) == ZeraMcontrollerBase::cmddone)
        {
            if (enable)
            {
                m_pGainCorrection->cmpCoefficients();
                m_pPhaseCorrection->cmpCoefficients();
                m_pOffsetCorrection->cmpCoefficients();
                return SCPI::scpiAnswer[SCPI::ack];
            }
            else
                return SCPI::scpiAnswer[SCPI::erraut];
        }
        else
            return SCPI::scpiAnswer[SCPI::errexec];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cCOM5003JustData::m_InitJustData(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isCommand(1) && (cmd.getParam(0) == ""))
    {
        bool enable;
        if (pAtmel->getEEPROMAccessEnable(enable) == ZeraMcontrollerBase::cmddone)
        {
            if (enable)
            {
                m_pGainCorrection->initJustData(1.0);
                m_pPhaseCorrection->initJustData(0.0);
                m_pOffsetCorrection->initJustData(0.0);
                return SCPI::scpiAnswer[SCPI::ack];
            }
            else
                return SCPI::scpiAnswer[SCPI::erraut];
        }
        else
            return SCPI::scpiAnswer[SCPI::errexec];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


void cCOM5003JustData::Serialize(QDataStream& qds)  // zum schreiben aller justagedaten in flashspeicher
{
    m_pGainCorrection->Serialize(qds); 
    m_pPhaseCorrection->Serialize(qds);
    m_pOffsetCorrection->Serialize(qds);
}
 

void cCOM5003JustData::Deserialize(QDataStream& qds) // zum lesen aller justagedaten aus flashspeicher
{
    m_pGainCorrection->Deserialize(qds); 
    m_pPhaseCorrection->Deserialize(qds);
    m_pOffsetCorrection->Deserialize(qds);
}


quint8 cCOM5003JustData::getAdjustmentStatus()
{
    return m_pGainCorrection->getStatus() & m_pPhaseCorrection->getStatus() & m_pOffsetCorrection->getStatus();

}


void cCOM5003JustData::initJustData()
{
    m_pGainCorrection->initJustData(1.0);
    m_pPhaseCorrection->initJustData(0.0);
    m_pOffsetCorrection->initJustData(0.0);
}


void cCOM5003JustData::computeJustData()
{
    m_pGainCorrection->cmpCoefficients();
    m_pPhaseCorrection->cmpCoefficients();
    m_pOffsetCorrection->cmpCoefficients();
}






