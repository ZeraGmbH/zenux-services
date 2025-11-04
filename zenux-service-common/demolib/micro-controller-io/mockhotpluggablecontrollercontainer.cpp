#include "mockhotpluggablecontrollercontainer.h"

void MockHotPluggableControllerContainer::startActualizeEmobControllers(quint16 bitmaskAvailable,
                                                                        const cSenseSettings *senseSettings,
                                                                        int msWaitForApplicationStart)
{
    Q_UNUSED(bitmaskAvailable)
    Q_UNUSED(msWaitForApplicationStart)
    if(m_fireSigControllersChanged) {
        QMetaObject::invokeMethod(this, "sigControllersChanged", Qt::QueuedConnection);
    }
}

HotControllerMap MockHotPluggableControllerContainer::getCurrentControllers()
{
    return m_controllers;
}

void MockHotPluggableControllerContainer::addEMOBController(const QString &channelMName)
{
    m_addedChannelMNames.append(channelMName);
    m_fireSigControllersChanged = true;
}
