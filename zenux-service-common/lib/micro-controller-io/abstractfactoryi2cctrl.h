#ifndef ABSTRACTFACTORYI2CCTRL_H
#define ABSTRACTFACTORYI2CCTRL_H

#include "abstracti2ccontrollers.h"
#include "atmelwatcherinterface.h"
#include <memory>

class AbstractFactoryI2cCtrl
{
public:
    enum ControllerTypes
    {
        CTRL_TYPE_RELAIS,
        CTRL_TYPE_SYSTEM,
        CTRL_TYPE_EMOB
    };
    virtual AtmelWatcherInterfacePtr createAtmelWatcher(QString devnode) = 0; // this is more a device-node thingy...

    virtual I2cCtrlCriticalStatusPtr getCriticalStatusController() = 0;
    virtual I2cCtrlEepromPermissionPtr getPermissionCheckController() = 0;
    virtual I2cCtrlCommonVersionsPtrUnique getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel = -1) = 0;
    virtual I2cCtrlDeviceIdentificationDataPtr getDeviceIdentificationController() = 0;
    virtual I2cCtrlAccumulatorPtr getAccumulatorController() = 0;
    virtual I2cCtrlRangesPtr getRangesController() = 0;
    virtual I2cCtrlMModePtr getMModeController() = 0;
    virtual I2cCtrlPllPtr getPllController() = 0;
    virtual I2cCtrlClampStatusPtr getClampStatusController() = 0;
};

typedef std::shared_ptr<AbstractFactoryI2cCtrl> FactoryControllerAbstractPtr;

#endif // ABSTRACTFACTORYI2CCTRL_H
