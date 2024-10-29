#ifndef TESTFACTORYI2CCTRLCRITICALSTATUS_H
#define TESTFACTORYI2CCTRLCRITICALSTATUS_H

#include "abstractfactoryi2cctrl.h"

class TestFactoryI2cCtrlCriticalStatus : public AbstractFactoryI2cCtrl
{
public:
    TestFactoryI2cCtrlCriticalStatus(quint16 initialCriticalStatus);
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
    struct TPersitentControllerData
    {
        quint16 m_criticalStatus = 0;
        quint16 m_criticalStatusMask = 0;
        QString m_serialNumber = "Demo Serial number";
        QString m_FPGAVersion = "Demo FPGA version";
        QString m_writablePcbVersion = "Unknown";

        quint8 m_pllChannel = 0;
    };
    static TPersitentControllerData m_persitentData;
};

#endif // TESTFACTORYI2CCTRLCRITICALSTATUS_H
