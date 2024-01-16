#ifndef MOCKATMELPLL_H
#define MOCKATMELPLL_H

#include "abstracti2ccontrollers.h"

class MockAtmelPll : public AtmelPll
{
public:
    MockAtmelPll(quint8& pllChannel);
    ZeraMControllerIo::atmelRM setPLLChannel(quint8 chn) override;
    ZeraMControllerIo::atmelRM readPLLChannel(quint8& chn) override;
private:
    quint8& m_pllChannel;
};

#endif // MOCKATMELPLL_H
