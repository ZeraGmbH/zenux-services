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
    I2cCtrlBootloaderPtr getBootloaderController() override;
private:
    SettingsContainerPtr m_settings;
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

#endif // DEMOFACTORYI2CCTRL_H
