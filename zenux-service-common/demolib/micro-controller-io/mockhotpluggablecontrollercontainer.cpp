#include "mockhotpluggablecontrollercontainer.h"

void MockHotPluggableControllerContainer::startActualizeEmobControllers(quint16 bitmaskAvailable,
                                                                        const cSenseSettings *senseSettings,
                                                                        int msWaitForApplicationStart)
{

}

QVector<I2cCtrlCommonInfoPtrShared> MockHotPluggableControllerContainer::getCurrentCommonControllers()
{
    return QVector<I2cCtrlCommonInfoPtrShared>();
}

QVector<I2cCtrlEMOBPtr> MockHotPluggableControllerContainer::getCurrentEmobControllers()
{
    return QVector<I2cCtrlEMOBPtr>();
}
