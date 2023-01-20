#include "permissionfunctions.h"

AtmelPermissionTemplate *PermissionFunctions::m_controller = nullptr;

const std::function<bool (bool &)> PermissionFunctions::checkControllerPin =
    [](bool &allow) {
        return m_controller->hasPermission(allow);
    };

const std::function<bool (bool &)> PermissionFunctions::allowAlways =
    [](bool &allow) {
        allow = true;
        return true;
};

void PermissionFunctions::setPermissionPinController(AtmelPermissionTemplate *controller)
{
    m_controller = controller;
}
