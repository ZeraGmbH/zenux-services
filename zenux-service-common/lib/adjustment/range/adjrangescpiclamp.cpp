#include "adjrangescpiclamp.h"

AdjRangeScpiClamp::AdjRangeScpiClamp(const std::shared_ptr<cSCPI> &scpiInterface,
                                     SenseRangeCommon *deviceInternalRange,
                                     double cvRatio,
                                     PermissionStructAdj permission)  :
    AdjRangeScpi(scpiInterface, AdjustScpiValueFormatterFactory::createMt310s2AdjFormatter(), permission),
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
