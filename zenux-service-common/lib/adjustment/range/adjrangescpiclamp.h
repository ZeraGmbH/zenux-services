#ifndef ADJRANGESCPICLAMP_H
#define ADJRANGESCPICLAMP_H

#include "adjrangescpi.h"
#include "senserangecommon.h"
#include "permissionfunctions.h"

class AdjRangeScpiClamp: public AdjRangeScpi
{
    Q_OBJECT
public:
    AdjRangeScpiClamp(const std::shared_ptr<cSCPI> &scpiInterface,
                      SenseRangeCommon* deviceInternalRange,
                      double cvRatio,
                      PermissionStructAdj permission = PermissionStructAdj());

protected:
    virtual double getGainCorrectionTotal(double par) override;
    virtual double getPhaseCorrectionTotal(double par) override;
    virtual double getOffsetCorrectionTotal(double par) override;

private:
    SenseRangeCommon* m_deviceInternalRange;
    double m_cvRatio;
};

#endif // ADJRANGESCPICLAMP_H

