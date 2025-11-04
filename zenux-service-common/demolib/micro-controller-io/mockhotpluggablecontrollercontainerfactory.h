#ifndef MOCKHOTPLUGGABLECONTROLLERCONTAINERFACTORY_H
#define MOCKHOTPLUGGABLECONTROLLERCONTAINERFACTORY_H

#include <abstracthotpluggablecontrollercontainerfactory.h>

class MockHotPluggableControllerContainerFactory : public AbstractHotPluggableControllerContainerFactory
{
public:
    AbstractHotPluggableControllerContainerPtr createHotplugContainer(I2cSettings *i2cSettings, AbstractFactoryI2cCtrlPtr ctrlFactory) override;
};

#endif // MOCKHOTPLUGGABLECONTROLLERCONTAINERFACTORY_H
