#ifndef CONTROLLERTYPENAME_H
#define CONTROLLERTYPENAME_H

#include "abstractfactoryi2cctrl.h"

class ControllerTypeName
{
public:
    static QString getCtrlTypeName(AbstractFactoryI2cCtrl::ControllerTypes ctrlType);
};

#endif // CONTROLLERTYPENAME_H
