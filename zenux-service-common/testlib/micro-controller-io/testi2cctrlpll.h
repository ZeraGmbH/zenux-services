#ifndef TESTI2CCTRLPLL_H
#define TESTI2CCTRLPLL_H

#include "abstractalli2ccontrollers.h"

class TestI2cCtrlPll : public AbstractI2cCtrlPll
{
public:
    TestI2cCtrlPll(quint8& pllChannel);
    ZeraMControllerIo::atmelRM setPLLChannel(quint8 chn) override;
    ZeraMControllerIo::atmelRM readPLLChannel(quint8& chn) override;
private:
    quint8& m_pllChannel;
};

#endif // TESTI2CCTRLPLL_H
