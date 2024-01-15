#include "permissionfunctions.h"

FactoryControllerAbstractPtr PermissionFunctions::m_ctrlFactory;

const FuncPermissionCheck PermissionFunctions::checkControllerPin =
    [](bool &allow) {
        return m_ctrlFactory->getPermissionCheckController()->hasPermission(allow);
    };

const FuncPermissionCheck PermissionFunctions::allowAlways =
    [](bool &allow) {
        allow = true;
        return true;
};

void PermissionFunctions::setPermissionCtrlFactory(FactoryControllerAbstractPtr ctrlFactory)
{
    m_ctrlFactory = ctrlFactory;
}
