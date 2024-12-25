#include "adjrangescpiclamp.h"

AdjRangeScpiClamp::AdjRangeScpiClamp(std::shared_ptr<cSCPI> scpiinterface,
                                     SenseRangeCommon *cascadedRange,
                                     double cvRatio,
                                     PermissionStructAdj permission)  :
    AdjRangeScpi(scpiinterface, AdjustScpiValueFormatterFactory::createMt310s2AdjFormatter(), permission),
    m_pFirstStageRange(cascadedRange),
    m_cvRatio(cvRatio)
{
}

double AdjRangeScpiClamp::getGainCorrectionTotal(double par)
{
    return getGainCorrectionSingle(par) * m_pFirstStageRange->getJustData()->getGainCorrectionSingle(par / m_cvRatio);
}

double AdjRangeScpiClamp::getPhaseCorrectionTotal(double par)
{
    return getPhaseCorrectionSingle(par) + m_pFirstStageRange->getJustData()->getPhaseCorrectionSingle(par);
}

double AdjRangeScpiClamp::getOffsetCorrectionTotal(double par)
{
    return getOffsetCorrectionSingle(par) + m_pFirstStageRange->getJustData()->getOffsetCorrectionSingle(par / m_cvRatio);
}
