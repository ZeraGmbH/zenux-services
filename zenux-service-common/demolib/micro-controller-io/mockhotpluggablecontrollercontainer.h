#ifndef MOCKHOTPLUGGABLECONTROLLERCONTAINER_H
#define MOCKHOTPLUGGABLECONTROLLERCONTAINER_H

#include <abstracthotpluggablecontrollercontainer.h>

class MockHotPluggableControllerContainer : public AbstractHotPluggableControllerContainer
{
    Q_OBJECT
public:
    void startActualizeEmobControllers(quint16 bitmaskAvailable, const cSenseSettings* senseSettings, int msWaitForApplicationStart) override;
    QVector<I2cCtrlCommonInfoPtrShared> getCurrentCommonControllers() override;
    QVector<I2cCtrlEMOBPtr> getCurrentEmobControllers() override;

    void setControllersToReturn(bool fireSigControllersChanged,
                                const QVector<I2cCtrlCommonInfoPtrShared> &commonControllers,
                                const QVector<I2cCtrlEMOBPtr> &emobControllers);
private:
    bool m_fireSigControllersChanged;
    QVector<I2cCtrlCommonInfoPtrShared> m_commonControllers;
    QVector<I2cCtrlEMOBPtr> m_emobControllers;
};

#endif // MOCKHOTPLUGGABLECONTROLLERCONTAINER_H
