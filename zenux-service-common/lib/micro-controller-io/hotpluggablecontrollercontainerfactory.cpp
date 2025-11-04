#include "hotpluggablecontrollercontainerfactory.h"
#include "hotpluggablecontrollercontainer.h"

HotPluggableControllerContainerFactory::HotPluggableControllerContainerFactory(I2cSettings *i2cSettings,
                                                                               AbstractFactoryI2cCtrlPtr ctrlFactory) :
    m_i2cSettings(i2cSettings),
    m_ctrlFactory(ctrlFactory)
{
}

AbstractHotPluggableControllerContainerPtr HotPluggableControllerContainerFactory::createHotplugContainer()
{
    return std::make_unique<HotPluggableControllerContainer>(m_i2cSettings, m_ctrlFactory);
}

