#ifndef DEMOI2CCTRLRANGES_H
#define DEMOI2CCTRLRANGES_H

#include "abstractalli2ccontrollers.h"

class DemoI2cCtrlRanges : public AbstractI2cCtrlRanges
{
public:
    DemoI2cCtrlRanges();
    ZeraMControllerIo::atmelRM readRange(quint8 channel, quint8& range) override;
    ZeraMControllerIo::atmelRM setRange(quint8 channel, quint8 range) override;
    void startCmdIfNotStarted() override {}
};

#endif // DEMOI2CCTRLRANGES_H
