#ifndef MOCKI2CCTRLEMOB_H
#define MOCKI2CCTRLEMOB_H

#include "abstractalli2ccontrollers.h"

class MockI2cCtrlEMOB : public AbstractI2cCtrlEMOB
{
public:
    ZeraMControllerIo::atmelRM sendPushbuttonPress() override;
    ZeraMControllerIo::atmelRM readEmobLockState(quint8 &status) override;
    ZeraMControllerIo::atmelRM readEmobInstrumentSubType(QString& answer) override;
};

#endif // MOCKI2CCTRLEMOB_H
