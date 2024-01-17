#include "mocki2cctrlcommonversions.h"
#include "zeramcontrolleriotemplate.h"

MockI2cCtrlCommonVersions::MockI2cCtrlCommonVersions()
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlCommonVersions::readCTRLVersion(QString &answer)
{
    answer = "Unknown";
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlCommonVersions::readPCBVersion(QString &answer)
{
    answer = "Unknown";
    return ZeraMControllerIo::atmelRM::cmddone;
}
