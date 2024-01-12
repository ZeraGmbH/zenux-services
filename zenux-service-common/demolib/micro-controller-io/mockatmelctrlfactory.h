#ifndef MOCKATMELCTRLFACTORY_H
#define MOCKATMELCTRLFACTORY_H

#include "atmelctrlfactoryinterface.h"

class MockAtmelCtrlFactory : public AtmelCtrlFactoryInterface
{
public:
    MockAtmelCtrlFactory(bool initialPermission);
    AtmelPermissionTemplatePtrU getPermissionCheckController() override;
    AtmelCommonVersionsPtrU getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel = -1) override;
    AtmelAccumulatorHandlerPtrU getAccumulatorController() override;
};

#endif // MOCKATMELCTRLFACTORY_H
