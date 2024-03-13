#ifndef TESTI2CCTRLRANGES_H
#define TESTI2CCTRLRANGES_H

#include "abstractalli2ccontrollers.h"

class TestI2cCtrlRanges : public AbstractI2cCtrlRanges
{
public:
    TestI2cCtrlRanges();
    ZeraMControllerIo::atmelRM readRange(quint8 channel, quint8& range) override;
    ZeraMControllerIo::atmelRM setRange(quint8 channel, quint8 range) override;
};

#endif // TESTI2CCTRLRANGES_H
