#ifndef TESTFACTORYI2CCTRL_H
#define TESTFACTORYI2CCTRL_H

#include "abstractfactoryi2cctrl.h"

class TestFactoryI2cCtrl : public AbstractFactoryI2cCtrl
{
public:
    TestFactoryI2cCtrl(bool initialPermission, const QString &commonInfoPrefix = "");
    void setRangeGetSetDelay(int rangeGetSetDelay);
    void simulateApplicationRunnung();
    void simulateBootloaderRunning();

    AbstractCtrlHeartbeatWaitPtr createCtrlHeartbeatWait(QString devnode) override;
    I2cCtrlBootloaderPtr getBootloaderController(ControllerTypes ctrlType, qint8 muxChannel = -1) override;

    I2cCtrlCriticalStatusPtr getCriticalStatusController() override;
    I2cCtrlEepromPermissionPtr getPermissionCheckController() override;
    I2cCtrlCommonInfoPtrUnique getCommonInfoController(ControllerTypes ctrlType, qint8 muxChannel = -1) override;
    I2cCtrlDeviceIdentPtr getDeviceIdentController() override;
    I2cCtrlAccumulatorPtr getAccuController() override;
    I2cCtrlRangesPtr getRangesController() override;
    I2cCtrlMModePtr getMModeController() override;
    I2cCtrlPllPtr getPllController() override;
    I2cCtrlClampStatusPtr getClampStatusController() override;
    I2cCtrlCpuTemperaturePtr getCpuTemperatureController() override;
    I2cCtrlEMOBPtr getEmobController(qint8 muxChannel) override;

protected:
    enum ControllerRunState {
        BOOTLOADER_RUNNING,
        APPLICATION_RUNNING
    };
    ControllerRunState m_controllerRunState = APPLICATION_RUNNING;
private:
    QString m_commonInfoPrefix;
    int m_rangeGetSetDelay = 0;
};

typedef std::shared_ptr<TestFactoryI2cCtrl> TestFactoryI2cCtrlPtr;

#endif // TESTFACTORYI2CCTRL_H
