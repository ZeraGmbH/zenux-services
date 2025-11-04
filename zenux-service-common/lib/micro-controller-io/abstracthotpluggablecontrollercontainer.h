#ifndef ABSTRACTHOTPLUGGABLECONTROLLERCONTAINER_H
#define ABSTRACTHOTPLUGGABLECONTROLLERCONTAINER_H

#include "abstractalli2ccontrollers.h"
#include "sensesettings.h"
#include <QObject>

struct HotControllers
{
    I2cCtrlCommonInfoPtrShared m_commonController;
    I2cCtrlEMOBPtr m_emobController;
};
typedef QMap<int /* ctrlChannel */, HotControllers> HotControllerMap;


class AbstractHotPluggableControllerContainer : public QObject
{
    Q_OBJECT
public:
    virtual void startActualizeEmobControllers(quint16 bitmaskAvailable, const cSenseSettings* senseSettings, int msWaitForApplicationStart) = 0;
    virtual HotControllerMap getCurrentControllers() = 0;
signals:
    void sigControllersChanged();
};

typedef std::shared_ptr<AbstractHotPluggableControllerContainer> AbstractHotPluggableControllerContainerPtr;

#endif // ABSTRACTHOTPLUGGABLECONTROLLERCONTAINER_H
