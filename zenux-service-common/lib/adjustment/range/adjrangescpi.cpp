#include "adjrangescpi.h"
#include "protonetcommand.h"
#include "adjdataitemscpi.h"
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

AdjRangeScpi::AdjRangeScpi(std::shared_ptr<cSCPI> scpiinterface,
                           std::unique_ptr<AdjustScpiValueFormatter> adjustmentFormatter,
                           PermissionStructAdj permissions) :
    ScpiConnection(scpiinterface),
    m_gainCorrection({m_scpiInterface, permissions.funcAllowAdjGain, adjustmentFormatter->m_correctionExportDigits},
                       &m_adjGroupData.m_gainAdjData),
    m_phaseCorrection({m_scpiInterface, permissions.funcAllowAdjPhase, adjustmentFormatter->m_correctionExportDigits},
                        &m_adjGroupData.m_phasAdjData),
    m_offsetCorrection({m_scpiInterface, permissions.funcAllowAdjOffset, adjustmentFormatter->m_correctionExportDigits},
                         &m_adjGroupData.m_offsAdjData),
    m_scpiQueryFormatter(std::move(adjustmentFormatter)),
    m_permissions(permissions)
{
}

void AdjRangeScpi::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "GAIN", SCPI::CmdwP , m_scpiInterface, GainTotal);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "ADJGAIN", SCPI::CmdwP , m_scpiInterface, GainSingle);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "PHASE", SCPI::isCmdwP, m_scpiInterface, PhaseTotal);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "ADJPHASE", SCPI::isCmdwP, m_scpiInterface, PhaseSingle);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "OFFSET", SCPI::isCmdwP, m_scpiInterface, OffsetTotal);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "ADJOFFSET", SCPI::isCmdwP, m_scpiInterface, OffsetSingle);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "STATUS", SCPI::isQuery, m_scpiInterface, DirectJustStatus);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "COMPUTE", SCPI::isCmdwP, m_scpiInterface, DirectJustCompute);
    addDelegate(QString("%1CORRECTION").arg(leadingNodes), "INIT", SCPI::isCmdwP, m_scpiInterface, DirectJustInit);

    connect(&m_gainCorrection, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_gainCorrection.initSCPIConnection(QString("%1CORRECTION:GAIN").arg(leadingNodes));
    connect(&m_phaseCorrection, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_phaseCorrection.initSCPIConnection(QString("%1CORRECTION:PHASE").arg(leadingNodes));
    connect(&m_offsetCorrection, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
    m_offsetCorrection.initSCPIConnection(QString("%1CORRECTION:OFFSET").arg(leadingNodes));
}

void AdjRangeScpi::setAdjGroupData(AdjDataRange groupData)
{
    m_adjGroupData = groupData;
}

AdjDataRange AdjRangeScpi::getAdjGroupData()
{
    return m_adjGroupData;
}

AdjDataItemScpi *AdjRangeScpi::getAdjInterface(QString name)
{
    if(name == "Gain")
        return &m_gainCorrection;
    if(name == "Phase")
        return &m_phaseCorrection;
    if(name == "Offset")
        return &m_offsetCorrection;
    return nullptr;
}

void AdjRangeScpi::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
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

QString AdjRangeScpi::scpiQueryGainCorrectionTotal(const QString &scpiInput)
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

QString AdjRangeScpi::scpiQueryGainCorrectionSingle(QString &scpiInput)
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

QString AdjRangeScpi::scpiQueryPhaseCorrectionTotal(QString& scpiInput)
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

QString AdjRangeScpi::scpiQueryPhaseCorrectionSingle(QString &scpiInput)
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

QString AdjRangeScpi::scpiQueryOffsetCorrectionTotal(QString& scpiInput)
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

QString AdjRangeScpi::scpiQueryOffsetCorrectionSingle(QString &scpiInput)
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

QString AdjRangeScpi::scpiQueryStatus(QString& scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery()) {
        return QString("%1").arg(getAdjustmentStatus80Mask());
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString AdjRangeScpi::scpiCmdComputeJustData(QString& scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if(cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        bool atLeastOneComputed = false;
        bool enable = false;
        if (m_permissions.funcAllowAdjGain(enable) && enable) {
            m_gainCorrection.getAdjItem()->computeCoefficientsFromNodes();
            atLeastOneComputed = true;
        }
        if (m_permissions.funcAllowAdjPhase(enable) && enable) {
            m_phaseCorrection.getAdjItem()->computeCoefficientsFromNodes();
            atLeastOneComputed = true;
        }
        if (m_permissions.funcAllowAdjOffset(enable) && enable) {
            m_offsetCorrection.getAdjItem()->computeCoefficientsFromNodes();
            atLeastOneComputed = true;
        }
        if(atLeastOneComputed)
            return ZSCPI::scpiAnswer[ZSCPI::ack];
        return ZSCPI::scpiAnswer[ZSCPI::erraut];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString AdjRangeScpi::scpiCmdInitJustData(QString &scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        if(initJustData())
            return ZSCPI::scpiAnswer[ZSCPI::ack];
        return ZSCPI::scpiAnswer[ZSCPI::erraut];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

quint8 AdjRangeScpi::getAdjustmentStatus80Mask()
{
    return m_gainCorrection.getStatus() & m_phaseCorrection.getStatus() & m_offsetCorrection.getStatus();
}

bool AdjRangeScpi::initJustData()
{
    bool atLeastOneTouched = false;
    bool enable = false;
    if (m_permissions.funcAllowAdjGain(enable) && enable) {
        m_adjGroupData.initGain();
        atLeastOneTouched = true;
    }
    if (m_permissions.funcAllowAdjPhase(enable) && enable) {
        m_adjGroupData.initPhase();
        atLeastOneTouched = true;
    }
    if (m_permissions.funcAllowAdjOffset(enable) && enable) {
        m_adjGroupData.initOffset();
        atLeastOneTouched = true;
    }
    return atLeastOneTouched;
}

bool AdjRangeScpi::computeJustData()
{
    bool atLeastOneComputed = false;
    bool enable = false;
    if (m_permissions.funcAllowAdjGain(enable) && enable) {
        m_gainCorrection.getAdjItem()->computeCoefficientsFromNodes();
        atLeastOneComputed = true;
    }
    if (m_permissions.funcAllowAdjPhase(enable) && enable) {
        m_phaseCorrection.getAdjItem()->computeCoefficientsFromNodes();
        atLeastOneComputed = true;
    }
    if (m_permissions.funcAllowAdjOffset(enable) && enable) {
        m_offsetCorrection.getAdjItem()->computeCoefficientsFromNodes();
        atLeastOneComputed = true;
    }
    return atLeastOneComputed;
}

double AdjRangeScpi::getGainCorrectionTotal(double par)
{
    return m_gainCorrection.getAdjItem()->getCorrection(par);
}

double AdjRangeScpi::getGainCorrectionSingle(double par)
{
    return m_gainCorrection.getAdjItem()->getCorrection(par);
}

double AdjRangeScpi::getPhaseCorrectionTotal(double par)
{
    return m_phaseCorrection.getAdjItem()->getCorrection(par);
}

double AdjRangeScpi::getPhaseCorrectionSingle(double par)
{
    return m_phaseCorrection.getAdjItem()->getCorrection(par);
}

double AdjRangeScpi::getOffsetCorrectionTotal(double par)
{
    return m_offsetCorrection.getAdjItem()->getCorrection(par);
}

double AdjRangeScpi::getOffsetCorrectionSingle(double par)
{
    return m_offsetCorrection.getAdjItem()->getCorrection(par);
}
