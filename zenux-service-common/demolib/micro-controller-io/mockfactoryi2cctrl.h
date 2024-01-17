#ifndef MOCKFACTORYI2CCTRL_H
#define MOCKFACTORYI2CCTRL_H

#include "abstractfactoryi2cctrl.h"

class MockFactoryI2cCtrl : public AbstractFactoryI2cCtrl
{
public:
    MockFactoryI2cCtrl(bool initialPermission);
    AtmelWatcherInterfacePtr createAtmelWatcher(QString devnode) override;
    AtmelCriticalStatusPtr getCriticalStatusController() override;
    AtmelPermissionTemplatePtrU getPermissionCheckController() override;
    AtmelCommonVersionsPtrU getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel = -1) override;
    AtmelDeviceIdentificationDataU getDeviceIdentificationController() override;
    AtmelAccumulatorHandlerPtrU getAccumulatorController() override;
    AtmelRangesPtrU getRangesController() override;
    AtmelMModesPtrU getMModeController() override;
    AtmelPllPtrU getPllController() override;
private:
    struct TPersitentControllerData
    {
        bool m_permission = false;
        quint16 m_criticalStatus = 0;
        quint16 m_criticalStatusMask = 0;
        QString m_deviceName = "Unknown";
        QString m_serialNumber = "Unknown";
        QString m_LCAVersion = "Unknown";
        QString m_writablePcbVersion = "Unknown";

        quint8 m_pllChannel = 0;
    };
    static TPersitentControllerData m_persitentData;
};

#endif // MOCKFACTORYI2CCTRL_H
