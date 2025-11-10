#ifndef TESTHOTPLUGCTRLFACTORYI2CCTRL_H
#define TESTHOTPLUGCTRLFACTORYI2CCTRL_H

#include "abstractfactoryi2cctrl.h"
#include "i2csettings.h"

class TestHotPlugCtrlFactoryI2cCtrl : public AbstractFactoryI2cCtrl
{
public:
    TestHotPlugCtrlFactoryI2cCtrl(I2cSettings *i2cSettings);

    I2cCtrlCommonInfoPtrUnique getCommonInfoController(ControllerTypes ctrlType, quint8 muxChannel = -1) override;

    int getCtrlInstanceCount();
    void prepareNextTestControllers(QVector<bool> respondOnVersionList);

    AbstractCtrlHeartbeatWaitPtr createCtrlHeartbeatWait(QString) override { return nullptr; }
    I2cCtrlCriticalStatusPtr getCriticalStatusController() override { return nullptr; }
    I2cCtrlEepromPermissionPtr getPermissionCheckController() override { return nullptr; }
    I2cCtrlDeviceIdentPtr getDeviceIdentController() override { return nullptr; }
    I2cCtrlAccumulatorPtr getAccuController() override { return nullptr; }
    I2cCtrlRangesPtr getRangesController() override { return nullptr; }
    I2cCtrlMModePtr getMModeController() override { return nullptr; }
    I2cCtrlPllPtr getPllController() override { return nullptr; }
    I2cCtrlClampStatusPtr getClampStatusController() override { return nullptr; }
    I2cCtrlCpuTemperaturePtr getCpuTemperatureController() override { return nullptr; }
    I2cCtrlEMOBPtr getEmobController(quint8) override { return nullptr; }
    I2cCtrlMt650ePtr getMt650eController(quint8 muxChannel) override { return nullptr; }
    I2cCtrlBootloaderPtr getBootloaderController() override { return nullptr; }
private:
    I2cSettings *m_i2cSettings;
    int m_ctrlInstanceCount = 0;
    QVector<bool> m_respondOnVersionList;
};

#endif // TESTHOTPLUGCTRLFACTORYI2CCTRL_H
