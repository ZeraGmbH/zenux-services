#include "demoi2cctrlcputemperature.h"

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlCpuTemperature::sendCpuTemperature(float &temperature)
{
    Q_UNUSED(temperature)
    return ZeraMControllerIo::atmelRM::cmddone;
}
