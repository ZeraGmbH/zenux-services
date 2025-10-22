#ifndef TESTI2CCTRLRANGES_H
#define TESTI2CCTRLRANGES_H

#include "abstractalli2ccontrollers.h"
#include <timersingleshotqt.h>

class TestI2cCtrlRanges : public AbstractI2cCtrlRanges
{
    Q_OBJECT
public:
    TestI2cCtrlRanges(int delayedResponse);
    ZeraMControllerIo::atmelRM readRange(quint8 channel, quint8& range) override;
    ZeraMControllerIo::atmelRM setRange(quint8 channel, quint8 range) override;
private:
    int m_delayedResponse;
    TimerTemplateQtPtr m_delayTimer;
};

#endif // TESTI2CCTRLRANGES_H
