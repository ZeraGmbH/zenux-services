#include "com5003justdata.h"
#include "protonetcommand.h"
#include "justdatainterface.h"
#include "zscpi_response_definitions.h"
#include <scpi.h>

Com5003JustRangeTripletOffsetGainPhase::Com5003JustRangeTripletOffsetGainPhase(cSCPI *scpiinterface, PermissionStructAdj permissions) :
    ScpiConnection(scpiinterface),
    m_permissions(permissions)
{
    m_pGainCorrection = new JustDataInterface({m_pSCPIInterface, GainCorrOrder, 1.0, permissions.funcAllowAdjGain, 8});
    m_pPhaseCorrection = new JustDataInterface({m_pSCPIInterface, PhaseCorrOrder, 0.0, permissions.funcAllowAdjPhase, 8});
    m_pOffsetCorrection =  new JustDataInterface({m_pSCPIInterface, OffsetCorrOrder, 0.0, permissions.funcAllowAdjOffset, 8});
}

Com5003JustRangeTripletOffsetGainPhase::~Com5003JustRangeTripletOffsetGainPhase()
{
    delete m_pGainCorrection; 
    delete m_pPhaseCorrection;
    delete m_pOffsetCorrection;
}

void Com5003JustRangeTripletOffsetGainPhase::initSCPIConnection(QString leadingNodes)
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

void Com5003JustRangeTripletOffsetGainPhase::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case DirectGain:
    case DirectJustGain:
        protoCmd->m_sOutput = scpiQueryGainCorrection(protoCmd->m_sInput);
        break;
    case DirectPhase:
    case DirectJustPhase:
        protoCmd->m_sOutput = scpiQueryPhaseCorrection(protoCmd->m_sInput);
        break;
    case DirectOffset:
    case DirectJustOffset:
        protoCmd->m_sOutput = scpiQueryOffsetCorrection(protoCmd->m_sInput);
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

QString Com5003JustRangeTripletOffsetGainPhase::scpiQueryGainCorrection(const QString &scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(m_pGainCorrection->getCorrection(par),0,'f',8);
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003JustRangeTripletOffsetGainPhase::scpiQueryJustGainCorrection(QString &scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getJustGainCorrection(par));
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003JustRangeTripletOffsetGainPhase::scpiQueryPhaseCorrection(QString& scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(m_pPhaseCorrection->getCorrection(par),0,'f',7);
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003JustRangeTripletOffsetGainPhase::scpiQueryJustPhaseCorrection(QString &scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getJustPhaseCorrection(par));
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003JustRangeTripletOffsetGainPhase::scpiQueryOffsetCorrection(QString& scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(m_pOffsetCorrection->getCorrection(par),0,'f',8);
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003JustRangeTripletOffsetGainPhase::scpiQueryJustOffsetCorrection(QString &scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return QString("%1").arg(getJustOffsetCorrection(par));
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003JustRangeTripletOffsetGainPhase::scpiQueryStatus(QString& scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery()) {
        return QString("%1").arg(getAdjustmentStatus());
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003JustRangeTripletOffsetGainPhase::scpiCmdComputeJustData(QString& scpiInput)
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

QString Com5003JustRangeTripletOffsetGainPhase::scpiCmdInitJustData(QString &scpiInput)
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

void Com5003JustRangeTripletOffsetGainPhase::Serialize(QDataStream& qds)  // zum schreiben aller justagedaten in flashspeicher
{
    m_pGainCorrection->Serialize(qds); 
    m_pPhaseCorrection->Serialize(qds);
    m_pOffsetCorrection->Serialize(qds);
}
 
void Com5003JustRangeTripletOffsetGainPhase::Deserialize(QDataStream& qds) // zum lesen aller justagedaten aus flashspeicher
{
    m_pGainCorrection->Deserialize(qds); 
    m_pPhaseCorrection->Deserialize(qds);
    m_pOffsetCorrection->Deserialize(qds);
}

quint8 Com5003JustRangeTripletOffsetGainPhase::getAdjustmentStatus()
{
    return m_pGainCorrection->getStatus() & m_pPhaseCorrection->getStatus() & m_pOffsetCorrection->getStatus();

}

void Com5003JustRangeTripletOffsetGainPhase::initJustData()
{
    m_pGainCorrection->initJustData(1.0);
    m_pPhaseCorrection->initJustData(0.0);
    m_pOffsetCorrection->initJustData(0.0);
}

void Com5003JustRangeTripletOffsetGainPhase::computeJustData()
{
    m_pGainCorrection->cmpCoefficients();
    m_pPhaseCorrection->cmpCoefficients();
    m_pOffsetCorrection->cmpCoefficients();
}

double Com5003JustRangeTripletOffsetGainPhase::getTotalGainCorrection(double par)
{
    return m_pGainCorrection->getCorrection(par);
}

double Com5003JustRangeTripletOffsetGainPhase::getJustGainCorrection(double par)
{
    return m_pGainCorrection->getCorrection(par);
}

double Com5003JustRangeTripletOffsetGainPhase::getTotalPhaseCorrection(double par)
{
    return m_pPhaseCorrection->getCorrection(par);
}

double Com5003JustRangeTripletOffsetGainPhase::getJustPhaseCorrection(double par)
{
    return m_pPhaseCorrection->getCorrection(par);
}

double Com5003JustRangeTripletOffsetGainPhase::getTotalOffsetCorrection(double par)
{
    return m_pOffsetCorrection->getCorrection(par);
}

double Com5003JustRangeTripletOffsetGainPhase::getJustOffsetCorrection(double par)
{
    return m_pOffsetCorrection->getCorrection(par);
}
