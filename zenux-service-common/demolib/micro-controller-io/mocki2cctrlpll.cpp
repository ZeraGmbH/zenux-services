#include "mocki2cctrlpll.h"

MockI2cCtrlPll::MockI2cCtrlPll(quint8 &pllChannel) :
    m_pllChannel(pllChannel)
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlPll::setPLLChannel(quint8 chn)
{
    m_pllChannel = chn;
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlPll::readPLLChannel(quint8 &chn)
{
    chn = m_pllChannel;
    return ZeraMControllerIo::atmelRM::cmddone;
}
