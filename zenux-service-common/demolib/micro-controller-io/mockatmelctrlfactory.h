#ifndef MOCKATMELCTRLFACTORY_H
#define MOCKATMELCTRLFACTORY_H

#include "atmelctrlfactoryinterface.h"

class MockAtmelCtrlFactory : public AtmelCtrlFactoryInterface
{
public:
    MockAtmelCtrlFactory(bool initialPermission);
    AtmelWatcherInterfacePtr createAtmelWatcher(QString devnode) override;
    AtmelPermissionTemplatePtrU getPermissionCheckController() override;
    AtmelCommonVersionsPtrU getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel = -1) override;
    AtmelDeviceIdentificationDataU getDeviceIdentificationController() override;
    AtmelAccumulatorHandlerPtrU getAccumulatorController() override;
private:
    struct TPersitentControllerData
    {
        bool m_permission = false;
        QString m_deviceName = "Unknown";
        QString m_serialNumber = "Unknown";
        QString m_LCAVersion = "Unknown";
        QString m_writablePcbVersion = "Unknown";
    };
    static TPersitentControllerData m_persitentData;
};

#endif // MOCKATMELCTRLFACTORY_H
