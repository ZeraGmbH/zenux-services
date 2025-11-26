#include "mocki2cctrlcommoninfo.h"
#include "controllertypename.h"

MockI2cCtrlCommonInfo::MockI2cCtrlCommonInfo(AbstractFactoryI2cCtrl::ControllerTypes ctrlType, quint8 muxChannel,
                                             const QString &commonInfoOverride) :
    m_muxChannel(muxChannel),
    m_ctrlType(ctrlType),
    m_commonInfoOverride(commonInfoOverride)
{
}

QString MockI2cCtrlCommonInfo::getVersionPrefix() const
{
    return m_commonInfoOverride + ControllerTypeName::getCtrlTypeName(m_ctrlType);
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlCommonInfo::readCTRLVersion(QString &answer)
{
    answer = !m_commonInfoOverride.isEmpty() ?
                 m_commonInfoOverride :
                 ControllerTypeName::getCtrlTypeName(m_ctrlType) + " Ctrl Version";
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlCommonInfo::readPCBInfo(QString &answer)
{
    answer = !m_commonInfoOverride.isEmpty() ?
                 m_commonInfoOverride :
                 ControllerTypeName::getCtrlTypeName(m_ctrlType) + " PCB Info";
    return ZeraMControllerIo::atmelRM::cmddone;
}
