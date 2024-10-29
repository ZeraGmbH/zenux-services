#ifndef FACTORYI2CCTRL_H
#define FACTORYI2CCTRL_H

#include "abstractfactoryi2cctrl.h"
#include "i2csettings.h"

class FactoryI2cCtrl : public AbstractFactoryI2cCtrl
{
public:
    FactoryI2cCtrl(I2cSettings *i2cSettings);

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
    I2cCtrlBootloaderPtr getBootloaderController() override;
    I2cCtrlCpuTemperaturePtr getCpuTemperatureController() override;
private:
    quint8 getRelaisCtrlI2cAddress();
    quint8 getSystemCtrlI2cAddress();
    quint8 getEmobCtrlI2cAddress();
    quint8 getEmobMuxI2cAddress();
    I2cSettings *m_i2cSettings;
    QString m_deviceNode;
    quint8 m_debugLevel;
};

#endif // FACTORYI2CCTRL_H
