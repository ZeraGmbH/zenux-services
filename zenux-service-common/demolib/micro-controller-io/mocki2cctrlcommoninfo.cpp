#include "mocki2cctrlcommoninfo.h"
#include "controllertypename.h"

MockI2cCtrlCommonInfo::MockI2cCtrlCommonInfo(AbstractFactoryI2cCtrl::ControllerTypes ctrlType, quint8 muxChannel,
                                             const QString &ctrlTypeNamePrefix) :
    m_muxChannel(muxChannel),
    m_ctrlType(ctrlType),
    m_ctrlTypeNamePrefix(ctrlTypeNamePrefix)
{
}

QString MockI2cCtrlCommonInfo::getVersionPrefix() const
{
    return m_ctrlTypeNamePrefix + "Demo " + ControllerTypeName::getCtrlTypeName(m_ctrlType);
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlCommonInfo::readCTRLVersion(QString &answer)
{
    answer = getVersionPrefix() + " Ctrl Version";
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlCommonInfo::readPCBInfo(QString &answer)
{
    answer = getVersionPrefix() + " PCB Info";
    return ZeraMControllerIo::atmelRM::cmddone;
}
