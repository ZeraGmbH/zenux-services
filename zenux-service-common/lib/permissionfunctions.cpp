#include "permissionfunctions.h"

AtmelPermissionTemplate *PermissionFunctions::m_controller = nullptr;

const FuncPermissionCheck PermissionFunctions::checkControllerPin =
    [](bool &allow) {
        return m_controller->hasPermission(allow);
    };

const FuncPermissionCheck PermissionFunctions::allowAlways =
    [](bool &allow) {
        allow = true;
        return true;
};

void PermissionFunctions::setPermissionPinController(AtmelPermissionTemplate *controller)
{
    m_controller = controller;
}
