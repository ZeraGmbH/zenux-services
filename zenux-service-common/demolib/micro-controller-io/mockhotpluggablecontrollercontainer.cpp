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

QVector<I2cCtrlCommonInfoPtrShared> MockHotPluggableControllerContainer::getCurrentCommonControllers()
{
    return m_commonControllers;
}

QVector<I2cCtrlEMOBPtr> MockHotPluggableControllerContainer::getCurrentEmobControllers()
{
    return m_emobControllers;
}

void MockHotPluggableControllerContainer::setControllersToReturn(bool fireSigControllersChanged,
                                                                 const QVector<I2cCtrlCommonInfoPtrShared> &commonControllers,
                                                                 const QVector<I2cCtrlEMOBPtr> &emobControllers)
{
    m_fireSigControllersChanged = fireSigControllersChanged;
    m_commonControllers = commonControllers;
    m_emobControllers = emobControllers;
}
