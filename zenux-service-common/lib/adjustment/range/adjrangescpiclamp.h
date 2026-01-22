#ifndef ADJRANGESCPICLAMP_H
#define ADJRANGESCPICLAMP_H

#include "adjrangescpi.h"
#include "senserangecommon.h"
#include "permissionfunctions.h"

class AdjRangeScpiClamp: public AdjRangeScpi
{
    Q_OBJECT
public:
    AdjRangeScpiClamp(std::shared_ptr<cSCPI> scpiinterface,
                      SenseRangeCommon* deviceInternalRange,
                      double cvRatio,
                      const PermissionStructAdj &permission = PermissionStructAdj());

protected:
    virtual double getGainCorrectionTotal(double par) override;
    virtual double getPhaseCorrectionTotal(double par) override;
    virtual double getOffsetCorrectionTotal(double par) override;

private:
    SenseRangeCommon* m_deviceInternalRange;
    double m_cvRatio;
};

#endif // ADJRANGESCPICLAMP_H

