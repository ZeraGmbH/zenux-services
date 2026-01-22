#include "adjrangescpiclamp.h"

AdjRangeScpiClamp::AdjRangeScpiClamp(std::shared_ptr<cSCPI> scpiinterface,
                                     SenseRangeCommon *deviceInternalRange,
                                     double cvRatio,
                                     const PermissionStructAdj &permission)  :
    AdjRangeScpi(scpiinterface, AdjustScpiValueFormatterFactory::createMt310s2AdjFormatter(), permission),
    m_deviceInternalRange(deviceInternalRange),
    m_cvRatio(cvRatio)
{
}

double AdjRangeScpiClamp::getGainCorrectionTotal(double par)
{
    return getGainCorrectionSingle(par) * m_deviceInternalRange->getJustData()->getGainCorrectionSingle(par / m_cvRatio);
}

double AdjRangeScpiClamp::getPhaseCorrectionTotal(double par)
{
    return getPhaseCorrectionSingle(par) + m_deviceInternalRange->getJustData()->getPhaseCorrectionSingle(par);
}

double AdjRangeScpiClamp::getOffsetCorrectionTotal(double par)
{
    return getOffsetCorrectionSingle(par) + m_deviceInternalRange->getJustData()->getOffsetCorrectionSingle(par / m_cvRatio);
}
