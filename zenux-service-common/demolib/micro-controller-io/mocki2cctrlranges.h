#ifndef MOCKI2CCTRLRANGES_H
#define MOCKI2CCTRLRANGES_H

#include "abstractalli2ccontrollers.h"

class MockI2cCtrlRanges : public AbstractI2cCtrlRanges
{
public:
    MockI2cCtrlRanges();
    ZeraMControllerIo::atmelRM readRange(quint8 channel, quint8& range) override;
    ZeraMControllerIo::atmelRM setRange(quint8 channel, quint8 range) override;
};

#endif // MOCKI2CCTRLRANGES_H
