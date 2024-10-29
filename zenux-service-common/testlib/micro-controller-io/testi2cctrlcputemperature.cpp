#include "testi2cctrlcputemperature.h"

#include "abstractalli2ccontrollers.h"

TestI2cCtrlCpuTmperature::TestI2cCtrlCpuTmperature() {}


ZeraMControllerIoTemplate::atmelRM TestI2cCtrlCpuTmperature::sendCpuTemperature(float &temperature)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

