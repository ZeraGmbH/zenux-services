#include "mockfactorycontroller.h"
#include "mockatmelwatcher.h"
#include "mockatmelcommonversions.h"
#include "mockatmelpermission.h"
#include "mockatmelaccumulatorhandler.h"
#include "mockatmeldeviceidentificationdata.h"
#include "mockatmelranges.h"
#include "mockatmelmmodes.h"

MockFactoryController::TPersitentControllerData MockFactoryController::m_persitentData;

MockFactoryController::MockFactoryController(bool initialPermission)
{
    m_persitentData.m_permission = initialPermission;
}

AtmelWatcherInterfacePtr MockFactoryController::createAtmelWatcher(QString devnode)
{
    Q_UNUSED(devnode)
    return std::make_unique<MockAtmelWatcher>();
}

AtmelPermissionTemplatePtrU MockFactoryController::getPermissionCheckController()
{
    return std::make_unique<MockAtmelPermission>(m_persitentData.m_permission);
}

AtmelCommonVersionsPtrU MockFactoryController::getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel)
{
    Q_UNUSED(ctrlType)
    Q_UNUSED(muxChannel)
    return std::make_unique<MockAtmelCommonVersions>(); // not prepared for updating pcbVersion write update...
}

AtmelDeviceIdentificationDataU MockFactoryController::getDeviceIdentificationController()
{
    return std::make_unique<MockAtmelDeviceIdentificationData>(m_persitentData.m_deviceName,
                                                               m_persitentData.m_serialNumber,
                                                               m_persitentData.m_LCAVersion,
                                                               m_persitentData.m_writablePcbVersion);
}

AtmelAccumulatorHandlerPtrU MockFactoryController::getAccumulatorController()
{
    return std::make_unique<MockAtmelAccumulatorHandler>();
}

AtmelRangesPtrU MockFactoryController::getRangesController()
{
    return std::make_unique<MockAtmelRanges>();
}

AtmelMModesPtrU MockFactoryController::getMModeController()
{
    return std::make_unique<MockAtmelMModes>();
}
