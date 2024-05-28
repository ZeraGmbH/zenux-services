#include "rangeadjclamps.h"

RangeAdjClamps::RangeAdjClamps(cSCPI *scpiinterface,
                               SenseRangeCommon *cascadedRange,
                               double cvRatio,
                               PermissionStructAdj permission)  :
    AdjRangeInterface(scpiinterface, AdjustScpiValueFormatterFactory::createMt310s2AdjFormatter(), permission),
    m_pFirstStageRange(cascadedRange),
    m_cvRatio(cvRatio)
{
}

double RangeAdjClamps::getGainCorrectionTotal(double par)
{
    return getGainCorrectionSingle(par) * m_pFirstStageRange->getJustData()->getGainCorrectionSingle(par / m_cvRatio);
}

double RangeAdjClamps::getPhaseCorrectionTotal(double par)
{
    return getPhaseCorrectionSingle(par) + m_pFirstStageRange->getJustData()->getPhaseCorrectionSingle(par);
}

double RangeAdjClamps::getOffsetCorrectionTotal(double par)
{
    return getOffsetCorrectionSingle(par) + m_pFirstStageRange->getJustData()->getOffsetCorrectionSingle(par / m_cvRatio);
}
