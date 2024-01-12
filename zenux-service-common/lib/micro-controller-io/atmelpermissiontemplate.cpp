#include "atmelcommon.h"

bool AtmelPermissionTemplate::hasPermission(bool &allow)
{
    return getEEPROMAccessEnable(allow) == ZeraMControllerIo::cmddone;
}
