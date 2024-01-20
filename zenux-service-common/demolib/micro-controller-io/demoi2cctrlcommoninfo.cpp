#include "demoi2cctrlcommoninfo.h"

DemoI2cCtrlCommonInfo::DemoI2cCtrlCommonInfo(QString ctrlTypeName) :
    m_prefix("Demo " + ctrlTypeName)
{
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlCommonInfo::readCTRLVersion(QString &answer)
{
    answer = m_prefix + " Ctrl Version";
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlCommonInfo::readPCBInfo(QString &answer)
{
    answer = m_prefix + " PCB Info";
    return ZeraMControllerIo::atmelRM::cmddone;
}
