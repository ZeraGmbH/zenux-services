#ifndef ATMELCTRLFACTORYINTERFACE_H
#define ATMELCTRLFACTORYINTERFACE_H

#include "i2csettings.h"
#include "atmelcommonversions.h"

class AtmelCtrlFactoryInterface
{
public:
    virtual AtmelCommonVersionsPtrS getVersionInfoGetter(i2cSettings::member ctrlType) = 0;
};

#endif // ATMELCTRLFACTORYINTERFACE_H
