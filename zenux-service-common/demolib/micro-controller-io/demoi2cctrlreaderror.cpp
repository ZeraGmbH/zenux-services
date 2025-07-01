#include "demoi2cctrlreaderror.h"
#include "simulsystemstatus.h"

DemoI2cCtrlReadError::DemoI2cCtrlReadError()
{
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlReadError::readError(QString &errorReply)
{
    // errorReply = ???
    return ZeraMControllerIo::atmelRM::cmddone;
}

