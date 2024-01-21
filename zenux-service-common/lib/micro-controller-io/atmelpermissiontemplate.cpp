#include "abstractalli2ccontrollers.h"

bool AbstractI2cCtrlEepromPermission::hasPermission(bool &allow)
{
    return getEEPROMAccessEnable(allow) == ZeraMControllerIo::cmddone;
}
