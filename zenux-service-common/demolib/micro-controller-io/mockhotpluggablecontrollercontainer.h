#ifndef MOCKHOTPLUGGABLECONTROLLERCONTAINER_H
#define MOCKHOTPLUGGABLECONTROLLERCONTAINER_H

#include "abstracthotpluggablecontrollercontainer.h"
#include "sensesettings.h"

class MockHotPluggableControllerContainer : public AbstractHotPluggableControllerContainer
{
    Q_OBJECT
public:
    void startActualizeEmobControllers(quint16 bitmaskAvailable,
                                       const cSenseSettings* senseSettings,
                                       int msWaitForApplicationStart) override;
    HotControllerMap getCurrentControllers() override;

    void addEMOBController(const QString& channelMName);
private:
    QStringList m_addedChannelMNames;
    bool m_fireSigControllersChanged;
    HotControllerMap m_controllers;
};

typedef std::shared_ptr<MockHotPluggableControllerContainer> MockHotPluggableControllerContainerPtr;

#endif // MOCKHOTPLUGGABLECONTROLLERCONTAINER_H
