#include "mockatmelpll.h"

MockAtmelPll::MockAtmelPll(quint8 &pllChannel) :
    m_pllChannel(pllChannel)
{
}

ZeraMControllerIoTemplate::atmelRM MockAtmelPll::setPLLChannel(quint8 chn)
{
    m_pllChannel = chn;
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockAtmelPll::readPLLChannel(quint8 &chn)
{
    chn = m_pllChannel;
    return ZeraMControllerIo::atmelRM::cmddone;
}
