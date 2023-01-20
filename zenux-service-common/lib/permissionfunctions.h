#ifndef PERMISSIONFUNCTIONS_H
#define PERMISSIONFUNCTIONS_H

#include "atmelpermissiontemplate.h"
#include <functional>

class PermissionFunctions
{
public:
    static void setPermissionPinController(AtmelPermissionTemplate *controller);
    const static std::function<bool (bool &)> checkControllerPin;
    const static std::function<bool (bool &)> allowAlways;
private:
    static AtmelPermissionTemplate *m_controller;
};

#endif // PERMISSIONFUNCTIONS_H
