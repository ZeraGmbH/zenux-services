#ifndef MOCKATMELRANGES_H
#define MOCKATMELRANGES_H

#include "abstracti2ccontrollers.h"

class MockAtmelRanges : public AtmelRanges
{
public:
    MockAtmelRanges();
    ZeraMControllerIo::atmelRM readRange(quint8 channel, quint8& range) override;
    ZeraMControllerIo::atmelRM setRange(quint8 channel, quint8 range) override;
};

#endif // MOCKATMELRANGES_H
