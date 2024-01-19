#ifndef HOTPLUGCTRLTESTFACTORY_H
#define HOTPLUGCTRLTESTFACTORY_H

#include "abstractfactoryi2cctrl.h"
#include "i2csettings.h"

class HotPlugCtrlTestFactory : public AbstractFactoryI2cCtrl
{
public:
    HotPlugCtrlTestFactory(I2cSettings *i2cSettings);

    I2cCtrlCommonVersionsPtrUnique getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel = -1) override;

    int getCtrlInstanceCount();
    void prepareNextTestControllers(QVector<bool> respondOnVersionList);

    AbstractCtrlHeartbeatWaitPtr createCtrlHeartbeatWait(QString) override { return nullptr; }
    I2cCtrlCriticalStatusPtr getCriticalStatusController() override { return nullptr; }
    I2cCtrlEepromPermissionPtr getPermissionCheckController() override { return nullptr; }
    I2cCtrlDeviceIdentificationDataPtr getDeviceIdentificationController() override { return nullptr; }
    I2cCtrlAccumulatorPtr getAccumulatorController() override { return nullptr; }
    I2cCtrlRangesPtr getRangesController() override { return nullptr; }
    I2cCtrlMModePtr getMModeController() override { return nullptr; }
    I2cCtrlPllPtr getPllController() override { return nullptr; }
    I2cCtrlClampStatusPtr getClampStatusController() override { return nullptr; }
    I2cCtrlBootloaderPtr getBootloaderController() override { return nullptr; }
private:
    I2cSettings *m_i2cSettings;
    int m_ctrlInstanceCount = 0;
    QVector<bool> m_respondOnVersionList;
};

#endif // HOTPLUGCTRLTESTFACTORY_H
