#ifndef PERMISSIONFUNCTIONS_H
#define PERMISSIONFUNCTIONS_H

#include "atmelctrlfactoryinterface.h"
#include <functional>

typedef std::function<bool (bool &)> FuncPermissionCheck;

class PermissionFunctions
{
public:
    static void setPermissionCtrlFactory(AtmelCtrlFactoryInterfacePrt ctrlFactory);
    const static FuncPermissionCheck checkControllerPin;
    const static FuncPermissionCheck allowAlways;
private:
    static AtmelCtrlFactoryInterfacePrt m_ctrlFactory;
};

struct PermissionStructAdj
{
    FuncPermissionCheck funcAllowAdjInit = PermissionFunctions::checkControllerPin;
    FuncPermissionCheck funcAllowAdjOffset = PermissionFunctions::checkControllerPin;
    FuncPermissionCheck funcAllowAdjGain = PermissionFunctions::checkControllerPin;
    FuncPermissionCheck funcAllowAdjPhase = PermissionFunctions::checkControllerPin;
    FuncPermissionCheck funcAllowAdjCompute = PermissionFunctions::checkControllerPin;
};

#endif // PERMISSIONFUNCTIONS_H
