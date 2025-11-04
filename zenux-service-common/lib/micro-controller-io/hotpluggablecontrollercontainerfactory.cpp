#include "hotpluggablecontrollercontainerfactory.h"
#include "hotpluggablecontrollercontainer.h"

AbstractHotPluggableControllerContainerPtr HotPluggableControllerContainerFactory::createHotplugContainer(I2cSettings *i2cSettings,
                                                                                                          AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    return std::make_unique<HotPluggableControllerContainer>(i2cSettings, ctrlFactory);
}
