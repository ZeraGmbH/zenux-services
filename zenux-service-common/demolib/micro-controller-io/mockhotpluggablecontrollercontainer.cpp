#include "mockhotpluggablecontrollercontainer.h"

void MockHotPluggableControllerContainer::startActualizeEmobControllers(quint16 bitmaskAvailable,
                                                                        const cSenseSettings *senseSettings,
                                                                        int msWaitForApplicationStart)
{
    Q_UNUSED(bitmaskAvailable)
    Q_UNUSED(senseSettings)
    Q_UNUSED(msWaitForApplicationStart)
    if(m_fireSigControllersChanged)
        QMetaObject::invokeMethod(this, "sigControllersChanged", Qt::QueuedConnection);
}

HotControllerMap MockHotPluggableControllerContainer::getCurrentControllers()
{
    return m_controllers;
}

void MockHotPluggableControllerContainer::setControllersForNextActualize(bool fireSigControllersChanged,
                                                                         const HotControllerMap &controllers)
{
    m_fireSigControllersChanged = fireSigControllersChanged;
    m_controllers = controllers;
}
