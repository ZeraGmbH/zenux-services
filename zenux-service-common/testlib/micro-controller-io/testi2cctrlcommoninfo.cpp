#include "testi2cctrlcommoninfo.h"

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlCommonInfo::readCTRLVersion(QString &answer)
{
    answer = "Unknown";
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlCommonInfo::readPCBInfo(QString &answer)
{
    answer = "Unknown";
    return ZeraMControllerIo::atmelRM::cmddone;
}
