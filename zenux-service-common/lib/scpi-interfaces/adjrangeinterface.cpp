#include "adjrangeinterface.h"
#include "protonetcommand.h"
#include "adjdataiteminterface.h"
#include "zscpi_response_definitions.h"
#include "adjdataitemstreamer.h"
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

AdjRangeInterface::AdjRangeInterface(cSCPI *scpiinterface,
                                                   std::unique_ptr<AdjustScpiValueFormatter> adjustmentFormatter,
                                                   PermissionStructAdj permissions) :
    ScpiConnection(scpiinterface),
    m_gainCorrection({m_pSCPIInterface, GainCorrOrder, 1.0, permissions.funcAllowAdjGain, adjustmentFormatter->m_correctionExportDigits},
                       &m_adjGroupData.m_gainAdjData),
    m_phaseCorrection({m_pSCPIInterface, PhaseCorrOrder, 0.0, permissions.funcAllowAdjPhase, adjustmentFormatter->m_correctionExportDigits},
                        &m_adjGroupData.m_phasAdjData),
    m_offsetCorrection({m_pSCPIInterface, OffsetCorrOrder, 0.0, permissions.funcAllowAdjOffset, adjustmentFormatter->m_correctionExportDigits},
                         &m_adjGroupData.m_offsAdjData),
    m_scpiQueryFormatter(std::move(adjustmentFormatter)),
    m_permissions(permissions)
{
}

void AdjRangeInterface::initSCPIConnection(QString leadingNodes)
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

    connect(&m_gainCorrection, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_gainCorrection.initSCPIConnection(QString("%1CORRECTION:GAIN").arg(leadingNodes));
    connect(&m_phaseCorrection, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_phaseCorrection.initSCPIConnection(QString("%1CORRECTION:PHASE").arg(leadingNodes));
    connect(&m_offsetCorrection, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_offsetCorrection.initSCPIConnection(QString("%1CORRECTION:OFFSET").arg(leadingNodes));
}

AdjDataItemInterface *AdjRangeInterface::getAdjInterface(QString name)
{
    if(name == "Gain")
        return &m_gainCorrection;
    if(name == "Phase")
        return &m_phaseCorrection;
    if(name == "Offset")
        return &m_offsetCorrection;
    return nullptr;
}

void AdjRangeInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
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

QString AdjRangeInterface::scpiQueryGainCorrectionTotal(const QString &scpiInput)
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

QString AdjRangeInterface::scpiQueryGainCorrectionSingle(QString &scpiInput)
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

QString AdjRangeInterface::scpiQueryPhaseCorrectionTotal(QString& scpiInput)
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

QString AdjRangeInterface::scpiQueryPhaseCorrectionSingle(QString &scpiInput)
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

QString AdjRangeInterface::scpiQueryOffsetCorrectionTotal(QString& scpiInput)
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

QString AdjRangeInterface::scpiQueryOffsetCorrectionSingle(QString &scpiInput)
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

QString AdjRangeInterface::scpiQueryStatus(QString& scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery()) {
        return QString("%1").arg(getAdjustmentStatus80Mask());
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString AdjRangeInterface::scpiCmdComputeJustData(QString& scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if(cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        bool enable;
        if(m_permissions.funcAllowAdjCompute(enable)) {
            if (enable) {
                m_gainCorrection.calcCoefficientsFromNodes();
                m_phaseCorrection.calcCoefficientsFromNodes();
                m_offsetCorrection.calcCoefficientsFromNodes();
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

QString AdjRangeInterface::scpiCmdInitJustData(QString &scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        bool enable;
        if(m_permissions.funcAllowAdjInit(enable)) {
            if (enable) {
                m_gainCorrection.initJustData(1.0);
                m_phaseCorrection.initJustData(0.0);
                m_offsetCorrection.initJustData(0.0);
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

void AdjRangeInterface::Serialize(QDataStream& qds)  // zum schreiben aller justagedaten in flashspeicher
{
    AdjDataItemStreamer::Serialize(*m_gainCorrection.getAdjItem(), qds);
    AdjDataItemStreamer::Serialize(*m_phaseCorrection.getAdjItem(), qds);
    AdjDataItemStreamer::Serialize(*m_offsetCorrection.getAdjItem(), qds);
}

void AdjRangeInterface::Deserialize(QDataStream& qds) // zum lesen aller justagedaten aus flashspeicher
{
    AdjDataItemStreamer::Deserialize(*m_gainCorrection.getAdjItem(), qds);
    AdjDataItemStreamer::Deserialize(*m_phaseCorrection.getAdjItem(), qds);
    AdjDataItemStreamer::Deserialize(*m_offsetCorrection.getAdjItem(), qds);
}

quint8 AdjRangeInterface::getAdjustmentStatus80Mask()
{
    return m_gainCorrection.getStatus() & m_phaseCorrection.getStatus() & m_offsetCorrection.getStatus();
}

void AdjRangeInterface::initJustData()
{
    m_gainCorrection.initJustData(1.0);
    m_phaseCorrection.initJustData(0.0);
    m_offsetCorrection.initJustData(0.0);
}

void AdjRangeInterface::computeJustData()
{
    m_gainCorrection.calcCoefficientsFromNodes();
    m_phaseCorrection.calcCoefficientsFromNodes();
    m_offsetCorrection.calcCoefficientsFromNodes();
}

double AdjRangeInterface::getGainCorrectionTotal(double par)
{
    return m_gainCorrection.getCorrection(par);
}

double AdjRangeInterface::getGainCorrectionSingle(double par)
{
    return m_gainCorrection.getCorrection(par);
}

double AdjRangeInterface::getPhaseCorrectionTotal(double par)
{
    return m_phaseCorrection.getCorrection(par);
}

double AdjRangeInterface::getPhaseCorrectionSingle(double par)
{
    return m_phaseCorrection.getCorrection(par);
}

double AdjRangeInterface::getOffsetCorrectionTotal(double par)
{
    return m_offsetCorrection.getCorrection(par);
}

double AdjRangeInterface::getOffsetCorrectionSingle(double par)
{
    return m_offsetCorrection.getCorrection(par);
}
