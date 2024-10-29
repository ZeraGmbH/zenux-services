#ifndef DEMOI2CCTRLCPUTEMPERATURE_H
#define DEMOI2CCTRLCPUTEMPERATURE_H

#include "abstractalli2ccontrollers.h"

class DemoI2cCtrlCpuTemperature : public AbstractI2cCtrlCpuTemperature
{
public:
    ZeraMControllerIo::atmelRM sendCpuTemperature(float &temperature) override;
};

#endif // DEMOI2CCTRLCPUTEMPERATURE_H
