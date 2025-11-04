#ifndef ABSTRACTHOTPLUGGABLECONTROLLERCONTAINER_H
#define ABSTRACTHOTPLUGGABLECONTROLLERCONTAINER_H

#include "abstractalli2ccontrollers.h"
#include "sensesettings.h"
#include <QObject>

class AbstractHotPluggableControllerContainer : public QObject
{
    Q_OBJECT
public:
    virtual void startActualizeEmobControllers(quint16 bitmaskAvailable, const cSenseSettings* senseSettings, int msWaitForApplicationStart) = 0;
    virtual QVector<I2cCtrlCommonInfoPtrShared> getCurrentCommonControllers() = 0;
    virtual QVector<I2cCtrlEMOBPtr> getCurrentEmobControllers() = 0;
signals:
    void sigControllersChanged();
};

#endif // ABSTRACTHOTPLUGGABLECONTROLLERCONTAINER_H
