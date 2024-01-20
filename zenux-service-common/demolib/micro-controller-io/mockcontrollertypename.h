#ifndef MOCKCONTROLLERTYPENAME_H
#define MOCKCONTROLLERTYPENAME_H

#include "abstractfactoryi2cctrl.h"

class MockControllerTypeName
{
public:
    static QString getCtrlTypeName(AbstractFactoryI2cCtrl::ControllerTypes ctrlType);
};

#endif // MOCKCONTROLLERTYPENAME_H
