#include "controllertypename.h"


QString ControllerTypeName::getCtrlTypeName(AbstractFactoryI2cCtrl::ControllerTypes ctrlType)
{
    switch(ctrlType) {
    default:
        return "Relais";
    case AbstractFactoryI2cCtrl::CTRL_TYPE_SYSTEM:
        return "System";
    case AbstractFactoryI2cCtrl::CTRL_TYPE_EMOB:
        return "Emob";
    }
}
