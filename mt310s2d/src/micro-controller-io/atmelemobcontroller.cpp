#include "atmelemobcontroller.h"

enum hw_cmdcode
{
    hwGetCtrlVersion = 0x0003,
};

AtmelEmobController::AtmelEmobController(QString devnode, quint8 adr, quint8 debuglevel) :
    ZeraMcontrollerBase(devnode, adr, debuglevel)
{
}

ZeraMcontrollerBase::atmelRM AtmelEmobController::readCTRLVersion(QString &answer)
{
    return readVariableLenText(hwGetCtrlVersion, answer);
}
