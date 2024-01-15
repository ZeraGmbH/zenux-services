#include "mockatmelctrlfactory.h"
#include "atmelwatchermock.h"
#include "mockatmelcommonversions.h"
#include "atmelpermissionmock.h"
#include "mockatmelaccumulatorhandler.h"

MockAtmelCtrlFactory::MockAtmelCtrlFactory(bool initialPermission)
{
    AtmelPermissionMock::setPermission(initialPermission);
}

AtmelWatcherInterfacePtr MockAtmelCtrlFactory::createAtmelWatcher(QString devnode)
{
    Q_UNUSED(devnode)
    return std::make_unique<AtmelWatcherMock>();
}

AtmelPermissionTemplatePtrU MockAtmelCtrlFactory::getPermissionCheckController()
{
    return AtmelPermissionMock::create();
}

AtmelCommonVersionsPtrU MockAtmelCtrlFactory::getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel)
{
    Q_UNUSED(ctrlType)
    Q_UNUSED(muxChannel)
    return std::make_unique<MockAtmelCommonVersions>();
}

AtmelAccumulatorHandlerPtrU MockAtmelCtrlFactory::getAccumulatorController()
{
    return std::make_unique<MockAtmelAccumulatorHandler>();
}
