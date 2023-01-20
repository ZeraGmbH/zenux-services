#ifndef ATMELPERMISSIONTEMPLATE_H
#define ATMELPERMISSIONTEMPLATE_H

#include <zera_mcontroller_base.h>

class AtmelPermissionTemplate
{
public:
    virtual ZeraMcontrollerBase::atmelRM getEEPROMAccessEnable(bool &enable) = 0;
    bool /*success*/ hasPermission(bool &allow);
};

#endif // ATMELPERMISSIONTEMPLATE_H
