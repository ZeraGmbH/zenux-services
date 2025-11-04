#ifndef ABSTRACTHOTPLUGGABLECONTROLLERCONTAINERFACTORY_H
#define ABSTRACTHOTPLUGGABLECONTROLLERCONTAINERFACTORY_H

#include "abstractfactoryi2cctrl.h"
#include "abstracthotpluggablecontrollercontainer.h"
#include "i2csettings.h"

class AbstractHotPluggableControllerContainerFactory
{
public:
    virtual ~AbstractHotPluggableControllerContainerFactory() = default;
    virtual AbstractHotPluggableControllerContainerPtr createHotplugContainer(I2cSettings *i2cSettings, AbstractFactoryI2cCtrlPtr ctrlFactory) = 0;
};

typedef std::shared_ptr<AbstractHotPluggableControllerContainerFactory> AbstractHotPluggableControllerContainerFactoryPtr;

#endif // ABSTRACTHOTPLUGGABLECONTROLLERCONTAINERFACTORY_H
