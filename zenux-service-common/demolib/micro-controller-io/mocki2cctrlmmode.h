#ifndef MOCKI2CCTRLMMODE_H
#define MOCKI2CCTRLMMODE_H

#include "abstractalli2ccontrollers.h"

class MockI2cCtrlMMode : public AbstractI2cCtrlMMode
{
public:
    MockI2cCtrlMMode();
    ZeraMControllerIo::atmelRM setMeasMode(quint8 mmode) override;
    ZeraMControllerIo::atmelRM readMeasMode(quint8& mmode) override;
};

#endif // MOCKI2CCTRLMMODE_H
