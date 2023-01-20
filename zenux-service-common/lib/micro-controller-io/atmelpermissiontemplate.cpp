#include "atmelpermissiontemplate.h"

bool AtmelPermissionTemplate::hasPermission(bool &permission)
{
    return getEEPROMAccessEnable(permission) == ZeraMcontrollerBase::cmddone;
}
