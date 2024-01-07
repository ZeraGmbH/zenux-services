#include "rangeadjustmentinterface.h"
#include "protonetcommand.h"
#include "justdatainterface.h"
#include "zscpi_response_definitions.h"
#include <scpi.h>

std::unique_ptr<AdjustScpiValueFormatter> AdjustScpiValueFormatterFactory::createMt310s2AdjFormatter()
{
    std::unique_ptr<AdjustScpiValueFormatter> adjustmentFormatter = std::make_unique<AdjustScpiValueFormatter>();
    adjustmentFormatter->m_gainScpiFormatter = [](double val) {
        return QString("%1").arg(val);
    };
    adjustmentFormatter->m_phaseScpiFormatter = [](double val) {
        return QString("%1").arg(val);
    };
    adjustmentFormatter->m_offsetScpiFormatter = [](double val) {
        return QString("%1").arg(val);
    };
    adjustmentFormatter->m_correctionExportDigits = 6;
    return adjustmentFormatter;
}

std::unique_ptr<AdjustScpiValueFormatter> AdjustScpiValueFormatterFactory::createCom5003AdjFormatter()
{
    std::unique_ptr<AdjustScpiValueFormatter> adjustmentFormatter = std::make_unique<AdjustScpiValueFormatter>();
    adjustmentFormatter->m_gainScpiFormatter = [](double val) {
        return QString("%1").arg(val, 0, 'f', 8);
    };
    adjustmentFormatter->m_phaseScpiFormatter = [](double val) {
        return QString("%1").arg(val, 0, 'f', 7);
    };
    adjustmentFormatter->m_offsetScpiFormatter = [](double val) {
        return QString("%1").arg(val, 0, 'f', 8);
    };
    adjustmentFormatter->m_correctionExportDigits = 8;
    return adjustmentFormatter;
}


enum ScpiCommands
{
    GainTotal,
    GainSingle,
    PhaseTotal,
    PhaseSingle,
    OffsetTotal,
    OffsetSingle,
    DirectJustStatus,
    DirectJustCompute,
    DirectJustInit
};

static constexpr int GainCorrOrder = 3; // ax^3 + bx^2 + cx + d
static constexpr int PhaseCorrOrder  = 3;
static constexpr int OffsetCorrOrder = 3;

RangeAdjustmentInterface::RangeAdjustmentInterface(cSCPI *scpiinterface,
                                                   std::unique_ptr<AdjustScpiValueFormatter> adjustmentFormatter,
                                                   PermissionStructAdj permissions) :
    ScpiConnection(scpiinterface),
    m_scpiQueryFormatter(std::move(adjustmentFormatter)),
    m_permissions(permissions)
{
    m_pGainCorrection = new JustDataInterface({m_pSCPIInterface, GainCorrOrder, 1.0, permissions.funcAllowAdjGain, m_scpiQueryFormatter->m_correctionExportDigits});
    m_pPhaseCorrection = new JustDataInterface({m_pSCPIInterface, PhaseCorrOrder, 0.0, permissions.funcAllowAdjPhase, m_scpiQueryFormatter->m_correctionExportDigits});
    m_pOffsetCorrection =  new JustDataInterface({m_pSCPIInterface, OffsetCorrOrder, 0.0, permissions.funcAllowAdjOffset, m_scpiQueryFormatter->m_correctionExportDigits});
}

RangeAdjustmentInterface::~RangeAdjustmentInterface()
{
    delete m_pGainCorrection;
    delete m_pPhaseCorrection;
    delete m_pOffsetCorrection;
}

void RangeAdjustmentInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "GAIN", SCPI::CmdwP , m_pSCPIInterface, GainTotal);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "ADJGAIN", SCPI::CmdwP , m_pSCPIInterface, GainSingle);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "PHASE", SCPI::isCmdwP, m_pSCPIInterface, PhaseTotal);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "ADJPHASE", SCPI::isCmdwP, m_pSCPIInterface, PhaseSingle);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "OFFSET", SCPI::isCmdwP, m_pSCPIInterface, OffsetTotal);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "ADJOFFSET", SCPI::isCmdwP, m_pSCPIInterface, OffsetSingle);
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

JustDataInterface *RangeAdjustmentInterface::getAdjInterface(QString name)
{
    if(name == "Gain")
        return m_pGainCorrection;
    if(name == "Phase")
        return m_pPhaseCorrection;
    if(name == "Offset")
        return m_pOffsetCorrection;
    qCritical("RangeAdjustmentInterface::getAdjInterface name '%s' not found", qPrintable(name));
    return nullptr;
}

void RangeAdjustmentInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case GainTotal:
        protoCmd->m_sOutput = scpiQueryGainCorrectionTotal(protoCmd->m_sInput);
        break;
    case GainSingle:
        protoCmd->m_sOutput = scpiQueryGainCorrectionSingle(protoCmd->m_sInput);
        break;
    case PhaseTotal:
        protoCmd->m_sOutput = scpiQueryPhaseCorrectionTotal(protoCmd->m_sInput);
        break;
    case PhaseSingle:
        protoCmd->m_sOutput = scpiQueryPhaseCorrectionSingle(protoCmd->m_sInput);
        break;
    case OffsetTotal:
        protoCmd->m_sOutput = scpiQueryOffsetCorrectionTotal(protoCmd->m_sInput);
        break;
    case OffsetSingle:
        protoCmd->m_sOutput = scpiQueryOffsetCorrectionSingle(protoCmd->m_sInput);
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

QString RangeAdjustmentInterface::scpiQueryGainCorrectionTotal(const QString &scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return m_scpiQueryFormatter->m_gainScpiFormatter(getGainCorrectionTotal(par));
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString RangeAdjustmentInterface::scpiQueryGainCorrectionSingle(QString &scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return m_scpiQueryFormatter->m_gainScpiFormatter(getGainCorrectionSingle(par));
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString RangeAdjustmentInterface::scpiQueryPhaseCorrectionTotal(QString& scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return m_scpiQueryFormatter->m_phaseScpiFormatter(getPhaseCorrectionTotal(par));
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString RangeAdjustmentInterface::scpiQueryPhaseCorrectionSingle(QString &scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return m_scpiQueryFormatter->m_phaseScpiFormatter(getPhaseCorrectionSingle(par));
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString RangeAdjustmentInterface::scpiQueryOffsetCorrectionTotal(QString& scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return m_scpiQueryFormatter->m_offsetScpiFormatter(getOffsetCorrectionTotal(par));
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString RangeAdjustmentInterface::scpiQueryOffsetCorrectionSingle(QString &scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery(1)) {
        QString spar = cmd.getParam(0);
        bool ok;
        double par = spar.toDouble(&ok);
        if (ok)
            return m_scpiQueryFormatter->m_offsetScpiFormatter(getOffsetCorrectionSingle(par));
        else
            return ZSCPI::scpiAnswer[ZSCPI::errval];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString RangeAdjustmentInterface::scpiQueryStatus(QString& scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery()) {
        return QString("%1").arg(getAdjustmentStatus());
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString RangeAdjustmentInterface::scpiCmdComputeJustData(QString& scpiInput)
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

QString RangeAdjustmentInterface::scpiCmdInitJustData(QString &scpiInput)
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

void RangeAdjustmentInterface::Serialize(QDataStream& qds)  // zum schreiben aller justagedaten in flashspeicher
{
    m_pGainCorrection->Serialize(qds);
    m_pPhaseCorrection->Serialize(qds);
    m_pOffsetCorrection->Serialize(qds);
}

void RangeAdjustmentInterface::Deserialize(QDataStream& qds) // zum lesen aller justagedaten aus flashspeicher
{
    m_pGainCorrection->Deserialize(qds);
    m_pPhaseCorrection->Deserialize(qds);
    m_pOffsetCorrection->Deserialize(qds);
}

quint8 RangeAdjustmentInterface::getAdjustmentStatus()
{
    return m_pGainCorrection->getStatus() & m_pPhaseCorrection->getStatus() & m_pOffsetCorrection->getStatus();

}

void RangeAdjustmentInterface::initJustData()
{
    m_pGainCorrection->initJustData(1.0);
    m_pPhaseCorrection->initJustData(0.0);
    m_pOffsetCorrection->initJustData(0.0);
}

void RangeAdjustmentInterface::computeJustData()
{
    m_pGainCorrection->cmpCoefficients();
    m_pPhaseCorrection->cmpCoefficients();
    m_pOffsetCorrection->cmpCoefficients();
}

double RangeAdjustmentInterface::getGainCorrectionTotal(double par)
{
    return m_pGainCorrection->getCorrection(par);
}

double RangeAdjustmentInterface::getGainCorrectionSingle(double par)
{
    return m_pGainCorrection->getCorrection(par);
}

double RangeAdjustmentInterface::getPhaseCorrectionTotal(double par)
{
    return m_pPhaseCorrection->getCorrection(par);
}

double RangeAdjustmentInterface::getPhaseCorrectionSingle(double par)
{
    return m_pPhaseCorrection->getCorrection(par);
}

double RangeAdjustmentInterface::getOffsetCorrectionTotal(double par)
{
    return m_pOffsetCorrection->getCorrection(par);
}

double RangeAdjustmentInterface::getOffsetCorrectionSingle(double par)
{
    return m_pOffsetCorrection->getCorrection(par);
}
