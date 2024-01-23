#include "demoi2cctrlaccu.h"
#include "simulsystemstatus.h"

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlAccu::readAccuStatus(quint8 &stat)
{
    stat = SimulSystemStatus::getInstance()->getAccuStatusFromFlags();
    return ZeraMControllerIoTemplate::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlAccu::readAccuStateOfCharge(quint8 &charge)
{
    charge = SimulSystemStatus::getInstance()->getAccuStateOfCharge();
    return ZeraMControllerIoTemplate::cmddone;
}
