#include "mocki2cctrlcommoninfo.h"
#include "controllertypename.h"

MockI2cCtrlCommonInfo::MockI2cCtrlCommonInfo(AbstractFactoryI2cCtrl::ControllerTypes ctrlType, quint8 muxChannel, const QString &ctrlTypeNamePrefix) :
    m_prefix(ctrlTypeNamePrefix + "Demo " + ControllerTypeName::getCtrlTypeName(ctrlType)),
    m_muxChannel(muxChannel)
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
