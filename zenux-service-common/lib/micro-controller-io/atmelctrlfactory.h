#ifndef ATMELCTRLFACTORY_H
#define ATMELCTRLFACTORY_H

#include "atmelctrlfactoryinterface.h"
#include "i2csettings.h"

class AtmelCtrlFactory : public AtmelCtrlFactoryInterface
{
public:
    AtmelCtrlFactory(cI2CSettings *i2cSettings);
    AtmelWatcherInterfacePtr createAtmelWatcher(QString devnode) override;
    AtmelPermissionTemplatePtrU getPermissionCheckController() override;
    AtmelCommonVersionsPtrU getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel = -1) override;
    AtmelAccumulatorHandlerPtrU getAccumulatorController() override;
private:
    cI2CSettings *m_i2cSettings;
};

#endif // ATMELCTRLFACTORY_H
