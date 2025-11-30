#ifndef PERMISSIONFUNCTIONS_H
#define PERMISSIONFUNCTIONS_H

#include "abstractfactoryi2cctrl.h"
#include <functional>

// return value: permission check succeeded
// param enable: yes we are allowed
typedef std::function<bool (bool &enable)> FuncPermissionCheck;

class PermissionFunctions
{
public:
    static void setPermissionCtrlFactory(AbstractFactoryI2cCtrlPtr ctrlFactory);
    const static FuncPermissionCheck checkControllerPin;
    const static FuncPermissionCheck allowAlways;
private:
    static AbstractFactoryI2cCtrlPtr m_ctrlFactory;
};

struct PermissionStructAdj
{
    FuncPermissionCheck funcAllowAdjOffset = PermissionFunctions::checkControllerPin;
    FuncPermissionCheck funcAllowAdjGain = PermissionFunctions::checkControllerPin;
    FuncPermissionCheck funcAllowAdjPhase = PermissionFunctions::checkControllerPin;
};

#endif // PERMISSIONFUNCTIONS_H
