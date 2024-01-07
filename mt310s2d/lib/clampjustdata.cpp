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
    return getGainCorrectionSingle(par) * m_pFirstStageRange->getJustData()->getGainCorrectionSingle(par / m_cvRatio);
}

double cClampJustData::getPhaseCorrectionTotal(double par)
{
    return getPhaseCorrectionSingle(par) + m_pFirstStageRange->getJustData()->getPhaseCorrectionSingle(par);
}

double cClampJustData::getOffsetCorrectionTotal(double par)
{
    return getOffsetCorrectionSingle(par) + m_pFirstStageRange->getJustData()->getOffsetCorrectionSingle(par / m_cvRatio);
}
