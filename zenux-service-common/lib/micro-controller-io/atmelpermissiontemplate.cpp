#include "abstracti2ccontrollers.h"

bool AtmelPermissionTemplate::hasPermission(bool &allow)
{
    return getEEPROMAccessEnable(allow) == ZeraMControllerIo::cmddone;
}
