#ifndef MOCKHOTPLUGGABLECONTROLLERCONTAINER_H
#define MOCKHOTPLUGGABLECONTROLLERCONTAINER_H

#include <abstracthotpluggablecontrollercontainer.h>

class MockHotPluggableControllerContainer : public AbstractHotPluggableControllerContainer
{
public:
    void startActualizeEmobControllers(quint16 bitmaskAvailable, const cSenseSettings* senseSettings, int msWaitForApplicationStart) override;
    QVector<I2cCtrlCommonInfoPtrShared> getCurrentCommonControllers() override;
    QVector<I2cCtrlEMOBPtr> getCurrentEmobControllers() override;
};

#endif // MOCKHOTPLUGGABLECONTROLLERCONTAINER_H
