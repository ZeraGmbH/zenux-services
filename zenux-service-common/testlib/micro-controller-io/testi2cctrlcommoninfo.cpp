#include "testi2cctrlcommoninfo.h"

TestI2cCtrlCommonInfo::TestI2cCtrlCommonInfo(AbstractFactoryI2cCtrl::ControllerTypes ctrlType, quint8 muxChannel) :
    m_ctrlType(ctrlType),
    m_muxChannel(muxChannel)
{
}

// 'Unknown' was default when we introduced setup test and recoreded regeression
// files. So changes here are expensive...
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
