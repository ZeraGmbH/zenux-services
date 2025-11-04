#ifndef HOTPLUGGABLECONTROLLERCONTAINERFACTORY_H
#define HOTPLUGGABLECONTROLLERCONTAINERFACTORY_H

#include "abstractfactoryi2cctrl.h"
#include "abstracthotpluggablecontrollercontainerfactory.h"
#include "i2csettings.h"

class HotPluggableControllerContainerFactory : public AbstractHotPluggableControllerContainerFactory
{
public:
    HotPluggableControllerContainerFactory(I2cSettings *i2cSettings, AbstractFactoryI2cCtrlPtr ctrlFactory);
    AbstractHotPluggableControllerContainerPtr createHotplugContainer() override;
private:
    I2cSettings *m_i2cSettings;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
};

#endif // HOTPLUGGABLECONTROLLERCONTAINERFACTORY_H
