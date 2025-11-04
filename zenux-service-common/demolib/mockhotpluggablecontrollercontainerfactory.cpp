#include "mockhotpluggablecontrollercontainerfactory.h"
#include "hotpluggablecontrollercontainer.h"

AbstractHotPluggableControllerContainerPtr MockHotPluggableControllerContainerFactory::createHotplugContainer(I2cSettings *i2cSettings,
                                                                                                              AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    // return production variant for now
    return std::make_unique<HotPluggableControllerContainer>(i2cSettings, ctrlFactory);
}
