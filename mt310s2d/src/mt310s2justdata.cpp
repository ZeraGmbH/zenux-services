#include "mt310s2justdata.h"
#include "protonetcommand.h"
#include "justdata.h"
#include "scpidelegate.h"
#include "atmel.h"
#include <scpi.h>

JustDataRangeGainPhaseOffset::JustDataRangeGainPhaseOffset(cSCPI *scpiinterface) :
    ScpiConnection(scpiinterface)
{
    bool(*checkPermission)(bool &enable) = [] (bool &enable) {
        return pAtmel->getEEPROMAccessEnable(enable) == ZeraMcontrollerBase::cmddone;
    };
    m_pGainCorrection = new cJustData({m_pSCPIInterface, GainCorrOrder, 1.0, checkPermission, 6});
    m_pPhaseCorrection = new cJustData({m_pSCPIInterface, PhaseCorrOrder, 0.0, checkPermission, 6});
    m_pOffsetCorrection =  new cJustData({m_pSCPIInterface, OffsetCorrOrder, 0.0, checkPermission, 6});
}

JustDataRangeGainPhaseOffset::~JustDataRangeGainPhaseOffset()
{
    delete m_pGainCorrection;
    delete m_pPhaseCorrection;
    delete m_pOffsetCorrection;
}

void JustDataRangeGainPhaseOffset::initSCPIConnection(QString leadingNodes)
{
    if (leadingNodes != "")
        leadingNodes += ":";
    cSCPIDelegate* delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "GAIN", SCPI::CmdwP , m_pSCPIInterface, DirectGain);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &JustDataRangeGainPhaseOffset::executeCommand);
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "ADJGAIN", SCPI::CmdwP , m_pSCPIInterface, DirectJustGain);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &JustDataRangeGainPhaseOffset::executeCommand);
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "PHASE", SCPI::isCmdwP, m_pSCPIInterface, DirectPhase);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &JustDataRangeGainPhaseOffset::executeCommand);
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "ADJPHASE", SCPI::isCmdwP, m_pSCPIInterface, DirectJustPhase);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &JustDataRangeGainPhaseOffset::executeCommand);
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "OFFSET", SCPI::isCmdwP, m_pSCPIInterface, DirectOffset);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &JustDataRangeGainPhaseOffset::executeCommand);
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "ADJOFFSET", SCPI::isCmdwP, m_pSCPIInterface, DirectJustOffset);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &JustDataRangeGainPhaseOffset::executeCommand);
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "STATUS", SCPI::isQuery, m_pSCPIInterface, DirectJustStatus);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &JustDataRangeGainPhaseOffset::executeCommand);
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "COMPUTE", SCPI::isCmdwP, m_pSCPIInterface, DirectJustCompute);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &JustDataRangeGainPhaseOffset::executeCommand);
    delegate = new cSCPIDelegate(QString("%1CORRECTION").arg(leadingNodes), "INIT", SCPI::isCmdwP, m_pSCPIInterface, DirectJustInit);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::execute, this, &JustDataRangeGainPhaseOffset::executeCommand);

    connect(m_pGainCorrection, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_pGainCorrection->initSCPIConnection(QString("%1CORRECTION:GAIN").arg(leadingNodes));
    connect(m_pPhaseCorrection, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_pPhaseCorrection->initSCPIConnection(QString("%1CORRECTION:PHASE").arg(leadingNodes));
    connect(m_pOffsetCorrection, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_pOffsetCorrection->initSCPIConnection(QString("%1CORRECTION:OFFSET").arg(leadingNodes));
}

void JustDataRangeGainPhaseOffset::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case DirectGain:
        protoCmd->m_sOutput = scpiGetGainCorrection(protoCmd->m_sInput);
        break;
    case DirectJustGain:
        protoCmd->m_sOutput = mReadJustGainCorrection(protoCmd->m_sInput);
        break;
    case DirectPhase:
        protoCmd->m_sOutput = mReadPhaseCorrection(protoCmd->m_sInput);
        break;
    case DirectJustPhase:
        protoCmd->m_sOutput = mReadJustPhaseCorrection(protoCmd->m_sInput);
        break;
    case DirectOffset:
        protoCmd->m_sOutput = mReadOffsetCorrection(protoCmd->m_sInput);
        break;
    case DirectJustOffset:
        protoCmd->m_sOutput = mReadJustOffsetCorrection(protoCmd->m_sInput);
        break;
    case DirectJustStatus:
        protoCmd->m_sOutput = m_ReadStatus(protoCmd->m_sInput);
        break;
    case DirectJustCompute:
        protoCmd->m_sOutput = m_ComputeJustData(protoCmd->m_sInput);
        break;
    case DirectJustInit:
        protoCmd->m_sOutput = m_InitJustData(protoCmd->m_sInput);
        break;
    }

    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString JustDataRangeGainPhaseOffset::scpiGetGainCorrection(const QString &scpiInput)
{
    bool ok;
    cSCPICommand cmd = scpiInput;

    if (cmd.isQuery(1))
    {
        QString spar = cmd.getParam(0);
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getGainCorrection(par));
        else
            return SCPI::scpiAnswer[SCPI::errval];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString JustDataRangeGainPhaseOffset::mReadJustGainCorrection(QString &sInput)
{
    bool ok;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery(1))
    {
        QString spar = cmd.getParam(0);
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getJustGainCorrection(par));
        else
            return SCPI::scpiAnswer[SCPI::errval];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString JustDataRangeGainPhaseOffset::mReadPhaseCorrection(QString& sInput)
{
    bool ok;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery(1))
    {
        QString spar = cmd.getParam(0);
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getPhaseCorrection(par));
        else
            return SCPI::scpiAnswer[SCPI::errval];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];

}


QString JustDataRangeGainPhaseOffset::mReadJustPhaseCorrection(QString &sInput)
{
    bool ok;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery(1))
    {
        QString spar = cmd.getParam(0);
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getJustPhaseCorrection(par));
        else
            return SCPI::scpiAnswer[SCPI::errval];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString JustDataRangeGainPhaseOffset::mReadOffsetCorrection(QString& sInput)
{
    bool ok;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery(1))
    {
        QString spar = cmd.getParam(0);
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getOffsetCorrection(par));
        else
            return SCPI::scpiAnswer[SCPI::errval];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];

}


QString JustDataRangeGainPhaseOffset::mReadJustOffsetCorrection(QString &sInput)
{
    bool ok;
    cSCPICommand cmd = sInput;

    if (cmd.isQuery(1))
    {
        QString spar = cmd.getParam(0);
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getJustOffsetCorrection(par));
        else
            return SCPI::scpiAnswer[SCPI::errval];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];

}


QString JustDataRangeGainPhaseOffset::m_ReadStatus(QString& sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        return QString("%1").arg(getAdjustmentStatus());
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString JustDataRangeGainPhaseOffset::m_ComputeJustData(QString& sInput)
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


QString JustDataRangeGainPhaseOffset::m_InitJustData(QString &sInput)
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


double JustDataRangeGainPhaseOffset::getGainCorrection(double par)
{
    return m_pGainCorrection->getCorrection(par);
}


double JustDataRangeGainPhaseOffset::getJustGainCorrection(double par)
{
    return m_pGainCorrection->getCorrection(par);
}


double JustDataRangeGainPhaseOffset::getPhaseCorrection(double par)
{
    return m_pPhaseCorrection->getCorrection(par);
}


double JustDataRangeGainPhaseOffset::getJustPhaseCorrection(double par)
{
    return m_pPhaseCorrection->getCorrection(par);
}


double JustDataRangeGainPhaseOffset::getOffsetCorrection(double par)
{
    return m_pOffsetCorrection->getCorrection(par);
}


double JustDataRangeGainPhaseOffset::getJustOffsetCorrection(double par)
{
    return m_pOffsetCorrection->getCorrection(par);
}


void JustDataRangeGainPhaseOffset::Serialize(QDataStream& qds)  // zum schreiben aller justagedaten in flashspeicher
{
    m_pGainCorrection->Serialize(qds);
    m_pPhaseCorrection->Serialize(qds);
    m_pOffsetCorrection->Serialize(qds);
}


void JustDataRangeGainPhaseOffset::Deserialize(QDataStream& qds) // zum lesen aller justagedaten aus flashspeicher
{
    m_pGainCorrection->Deserialize(qds);
    m_pPhaseCorrection->Deserialize(qds);
    m_pOffsetCorrection->Deserialize(qds);
}


quint8 JustDataRangeGainPhaseOffset::getAdjustmentStatus()
{
    return m_pGainCorrection->getStatus() & m_pPhaseCorrection->getStatus() & m_pOffsetCorrection->getStatus();

}


void JustDataRangeGainPhaseOffset::initJustData()
{
    m_pGainCorrection->initJustData(1.0);
    m_pPhaseCorrection->initJustData(0.0);
    m_pOffsetCorrection->initJustData(0.0);
}


void JustDataRangeGainPhaseOffset::computeJustData()
{
    m_pGainCorrection->cmpCoefficients();
    m_pPhaseCorrection->cmpCoefficients();
    m_pOffsetCorrection->cmpCoefficients();
}


