#include "mocki2cctrlclampstatus.h"

MockI2cCtrlClampStatus::MockI2cCtrlClampStatus(quint16 &clampConnectMask) :
    m_clampConnectMask(clampConnectMask)
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlClampStatus::readClampStatus(quint16 &stat)
{
    stat = m_clampConnectMask;
    return ZeraMControllerIo::atmelRM::cmddone;
}
