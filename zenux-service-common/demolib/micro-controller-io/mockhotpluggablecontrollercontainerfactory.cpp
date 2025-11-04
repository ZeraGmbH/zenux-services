#include "mockhotpluggablecontrollercontainerfactory.h"
#include "mockhotpluggablecontrollercontainer.h"

AbstractHotPluggableControllerContainerPtr MockHotPluggableControllerContainerFactory::createHotplugContainer(I2cSettings *i2cSettings,
                                                                                                              AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    Q_UNUSED(i2cSettings)
    Q_UNUSED(ctrlFactory)
    return std::make_unique<MockHotPluggableControllerContainer>();
}
