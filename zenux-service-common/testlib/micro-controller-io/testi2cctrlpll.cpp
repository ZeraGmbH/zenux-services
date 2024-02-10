#include "testi2cctrlpll.h"

TestI2cCtrlPll::TestI2cCtrlPll(quint8 &pllChannel) :
    m_pllChannel(pllChannel)
{
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlPll::setPLLChannel(quint8 chn)
{
    m_pllChannel = chn;
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlPll::readPLLChannel(quint8 &chn)
{
    chn = m_pllChannel;
    return ZeraMControllerIo::atmelRM::cmddone;
}
