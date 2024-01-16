#ifndef MOCKATMELPLL_H
#define MOCKATMELPLL_H

#include "atmelcommon.h"

class MockAtmelPll : public AtmelPll
{
public:
    MockAtmelPll(quint8& pllChannel);
    AtmelCommon::atmelRM setPLLChannel(quint8 chn) override;
    AtmelCommon::atmelRM readPLLChannel(quint8& chn) override;
private:
    quint8& m_pllChannel;
};

#endif // MOCKATMELPLL_H
