#include "mocki2cctrlcommoninfo.h"

MockI2cCtrlCommonInfo::MockI2cCtrlCommonInfo(QString ctrlTypeName) :
    m_prefix("Demo " + ctrlTypeName)
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlCommonInfo::readCTRLVersion(QString &answer)
{
    answer = m_prefix + " Ctrl Version";
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlCommonInfo::readPCBInfo(QString &answer)
{
    answer = m_prefix + " PCB Info";
    return ZeraMControllerIo::atmelRM::cmddone;
}
