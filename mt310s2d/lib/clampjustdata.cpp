#include "clampjustdata.h"

cClampJustData::cClampJustData(cSCPI *scpiinterface,
                               Mt310s2SenseRange *cascadedRange,
                               double cvRatio,
                               PermissionStructAdj permission)  :
    RangeAdjustmentInterface(scpiinterface, AdjustScpiValueFormatterFactory::createMt310s2AdjFormatter(), permission),
    m_pFirstStageRange(cascadedRange),
    m_cvRatio(cvRatio)
{
}

double cClampJustData::getGainCorrectionTotal(double par)
{
    return m_pGainCorrection->getCorrection(par) * m_pFirstStageRange->getJustData()->m_pGainCorrection->getCorrection(par / m_cvRatio);
}

double cClampJustData::getPhaseCorrectionTotal(double par)
{
    return m_pPhaseCorrection->getCorrection(par) + m_pFirstStageRange->getJustData()->m_pPhaseCorrection->getCorrection(par);
}

double cClampJustData::getOffsetCorrectionTotal(double par)
{
    return m_pOffsetCorrection->getCorrection(par) + m_pFirstStageRange->getJustData()->m_pOffsetCorrection->getCorrection(par / m_cvRatio);
}
