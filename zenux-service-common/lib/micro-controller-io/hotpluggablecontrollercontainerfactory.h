#ifndef HOTPLUGGABLECONTROLLERCONTAINERFACTORY_H
#define HOTPLUGGABLECONTROLLERCONTAINERFACTORY_H

#include "abstracthotpluggablecontrollercontainerfactory.h"

class HotPluggableControllerContainerFactory : public AbstractHotPluggableControllerContainerFactory
{
public:
    AbstractHotPluggableControllerContainerPtr createHotplugContainer(I2cSettings *i2cSettings, AbstractFactoryI2cCtrlPtr ctrlFactory) override;
};

#endif // HOTPLUGGABLECONTROLLERCONTAINERFACTORY_H
