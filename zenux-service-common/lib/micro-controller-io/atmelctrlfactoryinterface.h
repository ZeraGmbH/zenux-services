#ifndef ATMELCTRLFACTORYINTERFACE_H
#define ATMELCTRLFACTORYINTERFACE_H

#include "atmelcommon.h"
#include "atmelwatcherinterface.h"
#include <memory>

class AtmelCtrlFactoryInterface
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
    virtual AtmelAccumulatorHandlerPtrU getAccumulatorController() = 0;
};

typedef std::shared_ptr<AtmelCtrlFactoryInterface> AtmelCtrlFactoryInterfacePrt;

#endif // ATMELCTRLFACTORYINTERFACE_H
