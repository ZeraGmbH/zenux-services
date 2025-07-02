#include "demoi2cctrlreaderror.h"
#include "simulsystemstatus.h"

DemoI2cCtrlReadError::DemoI2cCtrlReadError()
{
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlReadError::readErrorSysCtrl(QString &errorReply)
{
    // errorReply = ???
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlReadError::readErrorRelCtrl(QString &errorReply)
{
    // errorReply = ???
    return ZeraMControllerIo::atmelRM::cmddone;
}
