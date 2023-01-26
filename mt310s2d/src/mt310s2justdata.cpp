#include "mt310s2justdata.h"
#include "protonetcommand.h"
#include "justdatainterface.h"
#include "scpidelegate.h"
#include <scpi.h>

JustRangeTripletOffsetGainPhase::JustRangeTripletOffsetGainPhase(cSCPI *scpiinterface, PermissionStructAdj permissions) :
    ScpiConnection(scpiinterface),
    m_permissions(permissions)
{
    m_pGainCorrection = new JustDataInterface({m_pSCPIInterface, GainCorrOrder, 1.0, permissions.funcAllowAdjGain, 6});
    m_pPhaseCorrection = new JustDataInterface({m_pSCPIInterface, PhaseCorrOrder, 0.0, permissions.funcAllowAdjPhase, 6});
    m_pOffsetCorrection =  new JustDataInterface({m_pSCPIInterface, OffsetCorrOrder, 0.0, permissions.funcAllowAdjOffset, 6});
}

JustRangeTripletOffsetGainPhase::~JustRangeTripletOffsetGainPhase()
{
    delete m_pGainCorrection;
    delete m_pPhaseCorrection;
    delete m_pOffsetCorrection;
}

void JustRangeTripletOffsetGainPhase::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "GAIN", SCPI::CmdwP , m_pSCPIInterface, DirectGain);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "ADJGAIN", SCPI::CmdwP , m_pSCPIInterface, DirectJustGain);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "PHASE", SCPI::isCmdwP, m_pSCPIInterface, DirectPhase);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "ADJPHASE", SCPI::isCmdwP, m_pSCPIInterface, DirectJustPhase);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "OFFSET", SCPI::isCmdwP, m_pSCPIInterface, DirectOffset);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "ADJOFFSET", SCPI::isCmdwP, m_pSCPIInterface, DirectJustOffset);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "STATUS", SCPI::isQuery, m_pSCPIInterface, DirectJustStatus);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "COMPUTE", SCPI::isCmdwP, m_pSCPIInterface, DirectJustCompute);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "INIT", SCPI::isCmdwP, m_pSCPIInterface, DirectJustInit);

    connect(m_pGainCorrection, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_pGainCorrection->initSCPIConnection(QString("%1CORRECTION:GAIN").arg(leadingNodes));
    connect(m_pPhaseCorrection, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_pPhaseCorrection->initSCPIConnection(QString("%1CORRECTION:PHASE").arg(leadingNodes));
    connect(m_pOffsetCorrection, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_pOffsetCorrection->initSCPIConnection(QString("%1CORRECTION:OFFSET").arg(leadingNodes));
}

void JustRangeTripletOffsetGainPhase::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
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

QString JustRangeTripletOffsetGainPhase::scpiGetGainCorrection(const QString &scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getGainCorrection(par));
        else
            return SCPI::scpiAnswer[SCPI::errval];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString JustRangeTripletOffsetGainPhase::mReadJustGainCorrection(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getJustGainCorrection(par));
        else
            return SCPI::scpiAnswer[SCPI::errval];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString JustRangeTripletOffsetGainPhase::mReadPhaseCorrection(QString& sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getPhaseCorrection(par));
        else
            return SCPI::scpiAnswer[SCPI::errval];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString JustRangeTripletOffsetGainPhase::mReadJustPhaseCorrection(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getJustPhaseCorrection(par));
        else
            return SCPI::scpiAnswer[SCPI::errval];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString JustRangeTripletOffsetGainPhase::mReadOffsetCorrection(QString& sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getOffsetCorrection(par));
        else
            return SCPI::scpiAnswer[SCPI::errval];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString JustRangeTripletOffsetGainPhase::mReadJustOffsetCorrection(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getJustOffsetCorrection(par));
        else
            return SCPI::scpiAnswer[SCPI::errval];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString JustRangeTripletOffsetGainPhase::m_ReadStatus(QString& sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        return QString("%1").arg(getAdjustmentStatus());
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}

QString JustRangeTripletOffsetGainPhase::m_ComputeJustData(QString& sInput)
{
    cSCPICommand cmd = sInput;
    if(cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        bool enable;
        if(m_permissions.funcAllowAdjCompute(enable)) {
            if (enable) {
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

QString JustRangeTripletOffsetGainPhase::m_InitJustData(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        bool enable;
        if(m_permissions.funcAllowAdjInit(enable)) {
            if (enable) {
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

void JustRangeTripletOffsetGainPhase::Serialize(QDataStream& qds)  // zum schreiben aller justagedaten in flashspeicher
{
    m_pGainCorrection->Serialize(qds);
    m_pPhaseCorrection->Serialize(qds);
    m_pOffsetCorrection->Serialize(qds);
}

void JustRangeTripletOffsetGainPhase::Deserialize(QDataStream& qds) // zum lesen aller justagedaten aus flashspeicher
{
    m_pGainCorrection->Deserialize(qds);
    m_pPhaseCorrection->Deserialize(qds);
    m_pOffsetCorrection->Deserialize(qds);
}

quint8 JustRangeTripletOffsetGainPhase::getAdjustmentStatus()
{
    return m_pGainCorrection->getStatus() & m_pPhaseCorrection->getStatus() & m_pOffsetCorrection->getStatus();

}

void JustRangeTripletOffsetGainPhase::initJustData()
{
    m_pGainCorrection->initJustData(1.0);
    m_pPhaseCorrection->initJustData(0.0);
    m_pOffsetCorrection->initJustData(0.0);
}

void JustRangeTripletOffsetGainPhase::computeJustData()
{
    m_pGainCorrection->cmpCoefficients();
    m_pPhaseCorrection->cmpCoefficients();
    m_pOffsetCorrection->cmpCoefficients();
}

double JustRangeTripletOffsetGainPhase::getGainCorrection(double par)
{
    return m_pGainCorrection->getCorrection(par);
}

double JustRangeTripletOffsetGainPhase::getJustGainCorrection(double par)
{
    return m_pGainCorrection->getCorrection(par);
}

double JustRangeTripletOffsetGainPhase::getPhaseCorrection(double par)
{
    return m_pPhaseCorrection->getCorrection(par);
}

double JustRangeTripletOffsetGainPhase::getJustPhaseCorrection(double par)
{
    return m_pPhaseCorrection->getCorrection(par);
}

double JustRangeTripletOffsetGainPhase::getOffsetCorrection(double par)
{
    return m_pOffsetCorrection->getCorrection(par);
}

double JustRangeTripletOffsetGainPhase::getJustOffsetCorrection(double par)
{
    return m_pOffsetCorrection->getCorrection(par);
}
