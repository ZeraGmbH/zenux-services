#ifndef FACTORYCONTROLLERABSTRACT_H
#define FACTORYCONTROLLERABSTRACT_H

#include "atmelcommon.h"
#include "atmelwatcherinterface.h"
#include <memory>

class FactoryControllerAbstract
{
public:
    enum ControllerTypes
    {
        CTRL_TYPE_RELAIS,
        CTRL_TYPE_SYSTEM,
        CTRL_TYPE_EMOB
    };
    virtual AtmelWatcherInterfacePtr createAtmelWatcher(QString devnode) = 0;
    virtual AtmelPermissionTemplatePtrU getPermissionCheckController() = 0;
    virtual AtmelCommonVersionsPtrU getCommonVersionController(ControllerTypes ctrlType, quint8 muxChannel = -1) = 0;
    virtual AtmelDeviceIdentificationDataU getDeviceIdentificationController() = 0;
    virtual AtmelAccumulatorHandlerPtrU getAccumulatorController() = 0;
    virtual AtmelRangesPtrU getRangesController() = 0;
    virtual AtmelMModesPtrU getMModeController() = 0;
    virtual AtmelPllPtrU getPllController() = 0;
};

typedef std::shared_ptr<FactoryControllerAbstract> FactoryControllerAbstractPtr;

#endif // FACTORYCONTROLLERABSTRACT_H
