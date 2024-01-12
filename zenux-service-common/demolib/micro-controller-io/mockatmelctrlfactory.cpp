#include "mockatmelctrlfactory.h"
#include "mockatmelcommonversions.h"
#include "atmelpermissionmock.h"
#include "mockatmelaccumulatorhandler.h"

MockAtmelCtrlFactory::MockAtmelCtrlFactory(bool initialPermission)
{
    AtmelPermissionMock::setPermission(initialPermission);
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
