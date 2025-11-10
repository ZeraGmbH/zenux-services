#include "mocki2cctrlmt650e.h"

MockI2cCtrlMt650e::MockI2cCtrlMt650e() {}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlMt650e::sendPushbuttonPress()
{
    return ZeraMControllerIo::atmelRM::cmddone;
}
