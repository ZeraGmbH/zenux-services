#ifndef MOCKI2CCTRLEMOB_H
#define MOCKI2CCTRLEMOB_H

#include "abstractalli2ccontrollers.h"

class MockI2cCtrlEMOB : public AbstractI2cCtrlEMOB
{
public:
    ZeraMControllerIo::atmelRM sendPushbuttonPress() override;
};

#endif // MOCKI2CCTRLEMOB_H
