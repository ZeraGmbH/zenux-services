#include "demoi2cctrlreaderror.h"
#include "simulsystemstatus.h"

DemoI2cCtrlReadError::DemoI2cCtrlReadError()
{
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlReadError::readErrorSystemCtrl(QString &errorReply)
{
    // errorReply = ???
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlReadError::readErrorRelayCtrl(QString &errorReply)
{
    // errorReply = ???
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlReadError::readErrorEmobCtrl(QString &errorReply)
{
    // errorReply = ???
    return ZeraMControllerIo::atmelRM::cmddone;
}
