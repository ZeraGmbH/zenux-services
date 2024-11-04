#include "demoi2cctrlcputemperature.h"
#include "simulsystemstatus.h"


ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlCpuTemperature::sendCpuTemperature(float &temperature)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

