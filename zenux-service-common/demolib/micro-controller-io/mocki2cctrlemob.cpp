#include "mocki2cctrlemob.h"

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEMOB::sendPushbuttonPress()
{
    return ZeraMControllerIo::atmelRM::cmddone;
}
