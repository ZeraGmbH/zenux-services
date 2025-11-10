#ifndef MOCKI2CCTRLMT650E_H
#define MOCKI2CCTRLMT650E_H

#include "abstractalli2ccontrollers.h"

class MockI2cCtrlMt650e : public AbstractI2cCtrlMt650e
{
public:
    MockI2cCtrlMt650e();
    ZeraMControllerIo::atmelRM sendPushbuttonPress() override;
};

#endif // MOCKI2CCTRLMT650E_H
