#include "mockatmelctrlfactory.h"
#include "atmelwatchermock.h"
#include "mockatmelcommonversions.h"
#include "atmelpermissionmock.h"
#include "mockatmelaccumulatorhandler.h"
#include "mockatmeldeviceidentificationdata.h"

MockAtmelCtrlFactory::TPersitentControllerData MockAtmelCtrlFactory::m_persitentData;

MockAtmelCtrlFactory::MockAtmelCtrlFactory(bool initialPermission)
{
    m_persitentData.m_permission = initialPermission;
}

AtmelWatcherInterfacePtr MockAtmelCtrlFactory::createAtmelWatcher(QString devnode)
{
    Q_UNUSED(devnode)
    return std::make_unique<AtmelWatcherMock>();
}

AtmelPermissionTemplatePtrU MockAtmelCtrlFactory::getPermissionCheckController()
{
    return std::make_unique<AtmelPermissionMock>(m_persitentData.m_permission);
}

AtmelCommonVersionsPtrU MockAtmelCtrlFactory::getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel)
{
    Q_UNUSED(ctrlType)
    Q_UNUSED(muxChannel)
    return std::make_unique<MockAtmelCommonVersions>(); // not prepared for unpdating pcbVersion write update...
}

AtmelDeviceIdentificationDataU MockAtmelCtrlFactory::getDeviceIdentificationController()
{
    return std::make_unique<MockAtmelDeviceIdentificationData>(m_persitentData.m_deviceName,
                                                               m_persitentData.m_serialNumber,
                                                               m_persitentData.m_LCAVersion,
                                                               m_persitentData.m_writablePcbVersion);
}

AtmelAccumulatorHandlerPtrU MockAtmelCtrlFactory::getAccumulatorController()
{
    return std::make_unique<MockAtmelAccumulatorHandler>();
}
