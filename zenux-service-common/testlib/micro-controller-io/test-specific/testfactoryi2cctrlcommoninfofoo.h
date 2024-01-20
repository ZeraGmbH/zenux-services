#ifndef TESTFACTORYI2CCTRLCOMMONINFOFOO_H
#define TESTFACTORYI2CCTRLCOMMONINFOFOO_H

#include "abstractfactoryi2cctrl.h"

class TestFactoryI2cCtrlCommonInfoFoo : public AbstractFactoryI2cCtrl
{
public:
    TestFactoryI2cCtrlCommonInfoFoo(bool initialPermission);
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
private:
    struct TPersitentControllerData
    {
        bool m_permission = false;
        quint16 m_criticalStatus = 0;
        quint16 m_criticalStatusMask = 0;
        QString m_deviceName = "Unknown";
        QString m_serialNumber = "Unknown";
        QString m_FPGAVersion = "Foo";
        QString m_writablePcbVersion = "Unknown";

        quint8 m_pllChannel = 0;
    };
    static TPersitentControllerData m_persitentData;
};

#endif // TESTFACTORYI2CCTRLCOMMONINFOFOO_H
