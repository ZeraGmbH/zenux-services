#ifndef ABSTRACTHOTPLUGGABLECONTROLLERCONTAINERFACTORY_H
#define ABSTRACTHOTPLUGGABLECONTROLLERCONTAINERFACTORY_H

#include "abstracthotpluggablecontrollercontainer.h"

class AbstractHotPluggableControllerContainerFactory
{
public:
    virtual ~AbstractHotPluggableControllerContainerFactory() = default;
    virtual AbstractHotPluggableControllerContainerPtr createHotplugContainer() = 0;
};

typedef std::shared_ptr<AbstractHotPluggableControllerContainerFactory> AbstractHotPluggableControllerContainerFactoryPtr;

#endif // ABSTRACTHOTPLUGGABLECONTROLLERCONTAINERFACTORY_H
