#include "mockatmelcommonversions.h"
#include "zeramcontrolleriotemplate.h"

MockAtmelCommonVersions::MockAtmelCommonVersions()
{
}

ZeraMControllerIoTemplate::atmelRM MockAtmelCommonVersions::readCTRLVersion(QString &answer)
{
    answer = "Unknown";
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockAtmelCommonVersions::readPCBVersion(QString &answer)
{
    answer = "Unknown";
    return ZeraMControllerIo::atmelRM::cmddone;
}
