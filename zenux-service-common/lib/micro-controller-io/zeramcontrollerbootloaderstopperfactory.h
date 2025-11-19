#ifndef ZERAMCONTROLLERBOOTLOADERSTOPPERFACTORY_H
#define ZERAMCONTROLLERBOOTLOADERSTOPPERFACTORY_H

#include "zeramcontrollerbootloaderstopperinterface.h"
#include "zeramcontrolleriotemplate.h"

class ZeraMControllerBootloaderStopperFactory
{
public:
    static ZeraMControllerBootloaderStopperPtr createBootloaderStopper(ZeraMcontrollerIoPtr i2cCtrl, int channelId);
protected:
    static ZeraMControllerBootloaderStopperPtr defaultCreate(ZeraMcontrollerIoPtr i2cCtrl, int channelId);
    static std::function<ZeraMControllerBootloaderStopperPtr(ZeraMcontrollerIoPtr i2cCtrl, int channelId)> m_createFunction;
};

#endif // ZERAMCONTROLLERBOOTLOADERSTOPPERFACTORY_H
