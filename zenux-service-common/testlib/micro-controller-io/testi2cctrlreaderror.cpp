#include "testi2cctrlreaderror.h"


TestI2cCtrlReadError::TestI2cCtrlReadError() {}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlReadError::readErrorSystemCtrl(QString &errorReply)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlReadError::readErrorRelayCtrl(QString &errorReply)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlReadError::readErrorEmobCtrl(QString &errorReply)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}
