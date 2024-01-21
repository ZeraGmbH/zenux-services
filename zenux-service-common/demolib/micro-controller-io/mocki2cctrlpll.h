#ifndef MOCKI2CCTRLPLL_H
#define MOCKI2CCTRLPLL_H

#include "abstractalli2ccontrollers.h"

class MockI2cCtrlPll : public AbstractI2cCtrlPll
{
public:
    MockI2cCtrlPll(quint8& pllChannel);
    ZeraMControllerIo::atmelRM setPLLChannel(quint8 chn) override;
    ZeraMControllerIo::atmelRM readPLLChannel(quint8& chn) override;
private:
    quint8& m_pllChannel;
};

#endif // MOCKI2CCTRLPLL_H
