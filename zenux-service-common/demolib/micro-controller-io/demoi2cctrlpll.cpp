#include "demoi2cctrlpll.h"

DemoI2cCtrlPll::DemoI2cCtrlPll(quint8 &pllChannel) :
    m_pllChannel(pllChannel)
{
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlPll::setPLLChannel(quint8 chn)
{
    m_pllChannel = chn;
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlPll::readPLLChannel(quint8 &chn)
{
    chn = m_pllChannel;
    return ZeraMControllerIo::atmelRM::cmddone;
}
