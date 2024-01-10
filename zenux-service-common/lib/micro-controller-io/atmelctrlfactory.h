#ifndef ATMELCTRLFACTORY_H
#define ATMELCTRLFACTORY_H

#include "atmelctrlfactoryinterface.h"

class AtmelCtrlFactory : public AtmelCtrlFactoryInterface
{
public:
    AtmelCtrlFactory(cI2CSettings *i2cSettings);
    AtmelCommonVersionsPtrS getVersionInfoGetter(i2cSettings::member ctrlType) override;
};

#endif // ATMELCTRLFACTORY_H
