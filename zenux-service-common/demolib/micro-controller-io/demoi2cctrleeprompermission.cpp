#include "demoi2cctrleeprompermission.h"
#include "simulsystemstatus.h"
#include <timerfactoryqt.h>

DemoI2cCtrlEepromPermission::DemoI2cCtrlEepromPermission()
{
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlEepromPermission::getEEPROMAccessEnable(bool &enable)
{
    enable = SimulSystemStatus::getInstance()->getSchnubbelPlugged();
    return ZeraMControllerIo::cmddone;
}
