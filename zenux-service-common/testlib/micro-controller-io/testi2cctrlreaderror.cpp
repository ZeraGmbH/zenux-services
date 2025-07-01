#include "testi2cctrlreaderror.h"


TestI2cCtrlReadError::TestI2cCtrlReadError() {}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlReadError::readError(QString &errorReplay)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

