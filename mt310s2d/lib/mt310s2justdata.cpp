#include "mt310s2justdata.h"
#include "protonetcommand.h"
#include "justdatainterface.h"
#include "zscpi_response_definitions.h"
#include <scpi.h>

Mt310s2JustRangeTripletOffsetGainPhase::Mt310s2JustRangeTripletOffsetGainPhase(cSCPI *scpiinterface, PermissionStructAdj permissions) :
    ScpiConnection(scpiinterface),
    m_permissions(permissions)
{
    m_pGainCorrection = new JustDataInterface({m_pSCPIInterface, GainCorrOrder, 1.0, permissions.funcAllowAdjGain, 6});
    m_pPhaseCorrection = new JustDataInterface({m_pSCPIInterface, PhaseCorrOrder, 0.0, permissions.funcAllowAdjPhase, 6});
    m_pOffsetCorrection =  new JustDataInterface({m_pSCPIInterface, OffsetCorrOrder, 0.0, permissions.funcAllowAdjOffset, 6});
}

Mt310s2JustRangeTripletOffsetGainPhase::~Mt310s2JustRangeTripletOffsetGainPhase()
{
    delete m_pGainCorrection;
    delete m_pPhaseCorrection;
    delete m_pOffsetCorrection;
}

void Mt310s2JustRangeTripletOffsetGainPhase::initSCPIConnection(QString leadingNodes)
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

void Mt310s2JustRangeTripletOffsetGainPhase::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case DirectGain:
        protoCmd->m_sOutput = scpiQueryGainCorrection(protoCmd->m_sInput);
        break;
    case DirectJustGain:
        protoCmd->m_sOutput = scpiQueryJustGainCorrection(protoCmd->m_sInput);
        break;
    case DirectPhase:
        protoCmd->m_sOutput = scpiQueryPhaseCorrection(protoCmd->m_sInput);
        break;
    case DirectJustPhase:
        protoCmd->m_sOutput = scpiQueryJustPhaseCorrection(protoCmd->m_sInput);
        break;
    case DirectOffset:
        protoCmd->m_sOutput = scpiQueryOffsetCorrection(protoCmd->m_sInput);
        break;
    case DirectJustOffset:
        protoCmd->m_sOutput = scpiQueryJustOffsetCorrection(protoCmd->m_sInput);
        break;
    case DirectJustStatus:
        protoCmd->m_sOutput = scpiQueryStatus(protoCmd->m_sInput);
        break;
    case DirectJustCompute:
        protoCmd->m_sOutput = scpiCmdComputeJustData(protoCmd->m_sInput);
        break;
    case DirectJustInit:
        protoCmd->m_sOutput = scpiCmdInitJustData(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}

QString Mt310s2JustRangeTripletOffsetGainPhase::scpiQueryGainCorrection(const QString &scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getGainCorrectionTotal(par));
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Mt310s2JustRangeTripletOffsetGainPhase::scpiQueryJustGainCorrection(QString &scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getGainCorrection(par));
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Mt310s2JustRangeTripletOffsetGainPhase::scpiQueryPhaseCorrection(QString& scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getPhaseCorrectionTotal(par));
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Mt310s2JustRangeTripletOffsetGainPhase::scpiQueryJustPhaseCorrection(QString &scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getPhaseCorrection(par));
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Mt310s2JustRangeTripletOffsetGainPhase::scpiQueryOffsetCorrection(QString& scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getOffsetCorrectionTotal(par));
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Mt310s2JustRangeTripletOffsetGainPhase::scpiQueryJustOffsetCorrection(QString &scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getOffsetCorrection(par));
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Mt310s2JustRangeTripletOffsetGainPhase::scpiQueryStatus(QString& scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery()) {
        return QString("%1").arg(getAdjustmentStatus());
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Mt310s2JustRangeTripletOffsetGainPhase::scpiCmdComputeJustData(QString& scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if(cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        bool enable;
        if(m_permissions.funcAllowAdjCompute(enable)) {
            if (enable) {
                m_pGainCorrection->cmpCoefficients();
                m_pPhaseCorrection->cmpCoefficients();
                m_pOffsetCorrection->cmpCoefficients();
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            else
                return ZSCPI::scpiAnswer[ZSCPI::erraut];
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Mt310s2JustRangeTripletOffsetGainPhase::scpiCmdInitJustData(QString &scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        bool enable;
        if(m_permissions.funcAllowAdjInit(enable)) {
            if (enable) {
                m_pGainCorrection->initJustData(1.0);
                m_pPhaseCorrection->initJustData(0.0);
                m_pOffsetCorrection->initJustData(0.0);
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            else
                return ZSCPI::scpiAnswer[ZSCPI::erraut];
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

void Mt310s2JustRangeTripletOffsetGainPhase::Serialize(QDataStream& qds)  // zum schreiben aller justagedaten in flashspeicher
{
    m_pGainCorrection->Serialize(qds);
    m_pPhaseCorrection->Serialize(qds);
    m_pOffsetCorrection->Serialize(qds);
}

void Mt310s2JustRangeTripletOffsetGainPhase::Deserialize(QDataStream& qds) // zum lesen aller justagedaten aus flashspeicher
{
    m_pGainCorrection->Deserialize(qds);
    m_pPhaseCorrection->Deserialize(qds);
    m_pOffsetCorrection->Deserialize(qds);
}

quint8 Mt310s2JustRangeTripletOffsetGainPhase::getAdjustmentStatus()
{
    return m_pGainCorrection->getStatus() & m_pPhaseCorrection->getStatus() & m_pOffsetCorrection->getStatus();

}

void Mt310s2JustRangeTripletOffsetGainPhase::initJustData()
{
    m_pGainCorrection->initJustData(1.0);
    m_pPhaseCorrection->initJustData(0.0);
    m_pOffsetCorrection->initJustData(0.0);
}

void Mt310s2JustRangeTripletOffsetGainPhase::computeJustData()
{
    m_pGainCorrection->cmpCoefficients();
    m_pPhaseCorrection->cmpCoefficients();
    m_pOffsetCorrection->cmpCoefficients();
}

double Mt310s2JustRangeTripletOffsetGainPhase::getGainCorrectionTotal(double par)
{
    return m_pGainCorrection->getCorrection(par);
}

double Mt310s2JustRangeTripletOffsetGainPhase::getGainCorrection(double par)
{
    return m_pGainCorrection->getCorrection(par);
}

double Mt310s2JustRangeTripletOffsetGainPhase::getPhaseCorrectionTotal(double par)
{
    return m_pPhaseCorrection->getCorrection(par);
}

double Mt310s2JustRangeTripletOffsetGainPhase::getPhaseCorrection(double par)
{
    return m_pPhaseCorrection->getCorrection(par);
}

double Mt310s2JustRangeTripletOffsetGainPhase::getOffsetCorrectionTotal(double par)
{
    return m_pOffsetCorrection->getCorrection(par);
}

double Mt310s2JustRangeTripletOffsetGainPhase::getOffsetCorrection(double par)
{
    return m_pOffsetCorrection->getCorrection(par);
}
