#ifndef FACTORYCONTROLLERATMEL_H
#define FACTORYCONTROLLERATMEL_H

#include "factorycontrollerabstract.h"
#include "i2csettings.h"

class FactoryControllerAtmel : public FactoryControllerAbstract
{
public:
    FactoryControllerAtmel(cI2CSettings *i2cSettings);

    AtmelWatcherInterfacePtr createAtmelWatcher(QString devnode) override;
    AtmelPermissionTemplatePtrU getPermissionCheckController() override;
    AtmelCommonVersionsPtrU getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel = -1) override;
    AtmelDeviceIdentificationDataU getDeviceIdentificationController() override;
    AtmelAccumulatorHandlerPtrU getAccumulatorController() override;
    AtmelRangesPtrU getRangesController() override;
    AtmelMModesPtrU getMModeController() override;
private:
    cI2CSettings *m_i2cSettings;
};

#endif // FACTORYCONTROLLERATMEL_H
