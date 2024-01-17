#ifndef FACTORYI2CCTRL_H
#define FACTORYI2CCTRL_H

#include "abstractfactoryi2cctrl.h"
#include "i2csettings.h"
#include "atmelctrlsystem.h"

class FactoryI2cCtrl : public AbstractFactoryI2cCtrl
{
public:
    FactoryI2cCtrl(cI2CSettings *i2cSettings);

    AtmelWatcherInterfacePtr createAtmelWatcher(QString devnode) override;
    I2cCtrlCriticalStatusPtr getCriticalStatusController() override;
    I2cCtrlEepromPermissionPtr getPermissionCheckController() override;
    I2cCtrlCommonVersionsPtrUnique getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel = -1) override;
    I2cCtrlDeviceIdentificationDataPtr getDeviceIdentificationController() override;
    I2cCtrlAccumulatorPtr getAccumulatorController() override;
    I2cCtrlRangesPtr getRangesController() override;
    I2cCtrlMModePtr getMModeController() override;
    I2cCtrlPllPtr getPllController() override;
    I2cCtrlClampStatusPtr getClampStatusController() override;
    I2cCtrlBootloaderPtr getBootloaderController() override;
private:
    quint8 getRelaisCtrlI2cAddress();
    quint8 getSystemCtrlI2cAddress();
    cI2CSettings *m_i2cSettings;
};

#endif // FACTORYI2CCTRL_H
