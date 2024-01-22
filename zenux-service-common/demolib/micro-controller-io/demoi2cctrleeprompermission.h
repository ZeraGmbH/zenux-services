#ifndef DEMOI2CCTRLEEPROMPERMISSION_H
#define DEMOI2CCTRLEEPROMPERMISSION_H

#include "abstractalli2ccontrollers.h"
#include <timertemplateqt.h>

class DemoI2cCtrlEepromPermission : public AbstractI2cCtrlEepromPermission
{
public:
    DemoI2cCtrlEepromPermission();

    ZeraMControllerIo::atmelRM getEEPROMAccessEnable(bool &enable) override;
};

#endif // DEMOI2CCTRLEEPROMPERMISSION_H
