#ifndef DEMOFACTORYI2CCTRL_H
#define DEMOFACTORYI2CCTRL_H

#include "abstractfactoryi2cctrl.h"
#include "settingscontainer.h"

class DemoFactoryI2cCtrl : public AbstractFactoryI2cCtrl
{
public:
    DemoFactoryI2cCtrl(SettingsContainerPtr settings);

    AbstractCtrlHeartbeatWaitPtr createCtrlHeartbeatWait(QString devnode) override;
    I2cCtrlCriticalStatusPtr getCriticalStatusController() override;
    I2cCtrlEepromPermissionPtr getPermissionCheckController() override;
    I2cCtrlCommonInfoPtrUnique getCommonInfoController(ControllerTypes ctrlType, quint8 muxChannel = -1) override;
    I2cCtrlDeviceIdentPtr getDeviceIdentController() override;
    I2cCtrlAccumulatorPtr getAccuController() override;
    I2cCtrlRangesPtr getRangesController() override;
    I2cCtrlMModePtr getMModeController() override;
    I2cCtrlPllPtr getPllController() override;
    I2cCtrlClampStatusPtr getClampStatusController() override;
    I2cCtrlCpuTemperaturePtr getCpuTemperatureController() override;
    I2cCtrlEMOBPtr getEmobController(quint8 muxChannel) override;
    I2cCtrlMt650ePtr getMt650eController(quint8 muxChannel) override;
    I2cCtrlBootloaderPtr getBootloaderController() override;
private:
    SettingsContainerPtr m_settings;
};

#endif // DEMOFACTORYI2CCTRL_H
