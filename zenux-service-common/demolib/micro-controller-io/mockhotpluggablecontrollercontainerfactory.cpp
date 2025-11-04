#include "mockhotpluggablecontrollercontainerfactory.h"
#include "mockhotpluggablecontrollercontainer.h"

AbstractHotPluggableControllerContainerPtr MockHotPluggableControllerContainerFactory::m_instance;

AbstractHotPluggableControllerContainerPtr MockHotPluggableControllerContainerFactory::createHotplugContainer()
{
    if (m_instance == nullptr)
        m_instance = std::make_shared<MockHotPluggableControllerContainer>();
    return m_instance;
}
