#include "mockatmel.h"

ZeraMcontrollerBase::atmelRM MockAtmel::getEEPROMAccessEnable(bool &enable)
{
    enable = true;
    return ZeraMcontrollerBase::cmddone;
}
