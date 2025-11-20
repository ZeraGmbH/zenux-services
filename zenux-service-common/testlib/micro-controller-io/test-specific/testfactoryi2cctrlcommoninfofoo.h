#ifndef TESTFACTORYI2CCTRLCOMMONINFOFOO_H
#define TESTFACTORYI2CCTRLCOMMONINFOFOO_H

#include "abstractfactoryi2cctrl.h"

class TestFactoryI2cCtrlCommonInfoFoo : public AbstractFactoryI2cCtrl
{
public:
    I2cCtrlCommonInfoPtrUnique getCommonInfoController(ControllerTypes ctrlType, quint8 muxChannel = -1) override;
    I2cCtrlDeviceIdentPtr getDeviceIdentController() override;
    I2cCtrlRangesPtr getRangesController() override;
    I2cCtrlMModePtr getMModeController() override;

    AbstractCtrlHeartbeatWaitPtr createCtrlHeartbeatWait(QString) override { return nullptr; }
    I2cCtrlCriticalStatusPtr getCriticalStatusController() override { return nullptr; }
    I2cCtrlEepromPermissionPtr getPermissionCheckController() override;
    I2cCtrlAccumulatorPtr getAccuController() override { return nullptr; }
    I2cCtrlPllPtr getPllController() override { return nullptr; }
    I2cCtrlClampStatusPtr getClampStatusController() override { return nullptr; }
    I2cCtrlCpuTemperaturePtr getCpuTemperatureController() override { return nullptr; }
    I2cCtrlEMOBPtr getEmobController(quint8) override { return nullptr; }
    I2cCtrlBootloaderPtr getBootloaderController(ControllerTypes, quint8) override { return nullptr; }
private:
    bool m_permission = true;
    struct TPersitentControllerData
    {
        QString m_serialNumber = "Unknown";
        QString m_writablePcbVersion = "Unknown";
    };
    static TPersitentControllerData m_persitentData;
};

#endif // TESTFACTORYI2CCTRLCOMMONINFOFOO_H
