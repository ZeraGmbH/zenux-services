#include "testi2cctrlreaderror.h"


TestI2cCtrlReadError::TestI2cCtrlReadError() {}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlReadError::readErrorSysCtrl(QString &errorReplay)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlReadError::readErrorRelCtrl(QString &errorReplay)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}
