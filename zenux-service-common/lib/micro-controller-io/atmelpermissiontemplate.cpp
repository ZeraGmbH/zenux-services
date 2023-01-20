#include "atmelpermissiontemplate.h"

bool AtmelPermissionTemplate::hasPermission(bool &allow)
{
    return getEEPROMAccessEnable(allow) == ZeraMcontrollerBase::cmddone;
}
