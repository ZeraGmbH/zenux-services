#ifndef MOCKHOTPLUGGABLECONTROLLERCONTAINERFACTORY_H
#define MOCKHOTPLUGGABLECONTROLLERCONTAINERFACTORY_H

#include <abstracthotpluggablecontrollercontainerfactory.h>

class MockHotPluggableControllerContainerFactory : public AbstractHotPluggableControllerContainerFactory
{
public:
    AbstractHotPluggableControllerContainerPtr createHotplugContainer() override;
private:
    static AbstractHotPluggableControllerContainerPtr m_instance;
};

#endif // MOCKHOTPLUGGABLECONTROLLERCONTAINERFACTORY_H
