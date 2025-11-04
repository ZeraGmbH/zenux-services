#ifndef MOCKHOTPLUGGABLECONTROLLERCONTAINER_H
#define MOCKHOTPLUGGABLECONTROLLERCONTAINER_H

#include <abstracthotpluggablecontrollercontainer.h>

class MockHotPluggableControllerContainer : public AbstractHotPluggableControllerContainer
{
    Q_OBJECT
public:
    void startActualizeEmobControllers(quint16 bitmaskAvailable,
                                       const cSenseSettings* senseSettings,
                                       int msWaitForApplicationStart) override;
    HotControllerMap getCurrentControllers() override;

    void setControllersForNextActualize(bool fireSigControllersChanged,
                                        const HotControllerMap &controllers);
private:
    bool m_fireSigControllersChanged;
    HotControllerMap m_controllers;
};

#endif // MOCKHOTPLUGGABLECONTROLLERCONTAINER_H
