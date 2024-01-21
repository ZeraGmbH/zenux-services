#ifndef ABSTRACTFACTORYI2CCTRL_H
#define ABSTRACTFACTORYI2CCTRL_H

#include "abstractalli2ccontrollers.h"
#include "abstractctrlheartbeatwait.h"
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
    virtual AbstractCtrlHeartbeatWaitPtr createCtrlHeartbeatWait(QString devnode) = 0; // this is more a device-node thingy...

    virtual I2cCtrlCriticalStatusPtr getCriticalStatusController() = 0;
    virtual I2cCtrlEepromPermissionPtr getPermissionCheckController() = 0;
    virtual I2cCtrlCommonInfoPtrUnique getCommonInfoController(ControllerTypes ctrlType, quint8 muxChannel = -1) = 0;
    virtual I2cCtrlDeviceIdentPtr getDeviceIdentController() = 0;
    virtual I2cCtrlAccumulatorPtr getAccuController() = 0;
    virtual I2cCtrlRangesPtr getRangesController() = 0;
    virtual I2cCtrlMModePtr getMModeController() = 0;
    virtual I2cCtrlPllPtr getPllController() = 0;
    virtual I2cCtrlClampStatusPtr getClampStatusController() = 0;
    // TODO: Merge zera-microcontroller bootloader here
    virtual I2cCtrlBootloaderPtr getBootloaderController() = 0;
};

typedef std::shared_ptr<AbstractFactoryI2cCtrl> AbstractFactoryI2cCtrlPtr;

#endif // ABSTRACTFACTORYI2CCTRL_H
