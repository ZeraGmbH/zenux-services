#include "testi2cctrlcommoninfofoo.h"

TestI2cCtrlCommonInfoFoo::TestI2cCtrlCommonInfoFoo(QString ctrlTypeName) :
    m_prefix("Foo " + ctrlTypeName)
{
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlCommonInfoFoo::readCTRLVersion(QString &answer)
{
    answer = m_prefix + " Ctrl Version";
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlCommonInfoFoo::readPCBInfo(QString &answer)
{
    answer = m_prefix + " PCB Info";
    return ZeraMControllerIo::atmelRM::cmddone;
}
