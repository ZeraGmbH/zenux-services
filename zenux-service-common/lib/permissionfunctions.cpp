#include "permissionfunctions.h"

AbstractFactoryI2cCtrlPtr PermissionFunctions::m_ctrlFactory;

const FuncPermissionCheck PermissionFunctions::checkControllerPin =
    [](bool &allow) {
        return m_ctrlFactory->getPermissionCheckController()->hasPermission(allow);
    };

const FuncPermissionCheck PermissionFunctions::allowAlways =
    [](bool &allow) {
        allow = true;
        return true;
};

void PermissionFunctions::setPermissionCtrlFactory(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    m_ctrlFactory = ctrlFactory;
}
