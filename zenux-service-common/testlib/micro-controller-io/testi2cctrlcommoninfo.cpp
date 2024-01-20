#include "testi2cctrlcommoninfo.h"

TestI2cCtrlCommonInfo::TestI2cCtrlCommonInfo(QString ctrlTypeName) :
    m_prefix("Test " + ctrlTypeName)
{
}

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
