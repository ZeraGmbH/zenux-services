#ifndef PERMISSIONFUNCTIONS_H
#define PERMISSIONFUNCTIONS_H

#include "atmelpermissiontemplate.h"
#include <functional>

typedef std::function<bool (bool &)> FuncPermissionCheck;

class PermissionFunctions
{
public:
    static void setPermissionPinController(AtmelPermissionTemplate *controller);
    const static FuncPermissionCheck checkControllerPin;
    const static FuncPermissionCheck allowAlways;
private:
    static AtmelPermissionTemplate *m_controller;
};

#endif // PERMISSIONFUNCTIONS_H
