#ifndef TESTI2CCTRLCPUTEMPERATURE_H
#define TESTI2CCTRLCPUTEMPERATURE_H

#include "abstractalli2ccontrollers.h"

class TestI2cCtrlCpuTmperature : public AbstractI2cCtrlCpuTemperature
{
public:
    TestI2cCtrlCpuTmperature();
    ZeraMControllerIo::atmelRM sendCpuTemperature(float &temperature) override;
};

#endif // TESTI2CCTRLCPUTEMPERATURE_H
