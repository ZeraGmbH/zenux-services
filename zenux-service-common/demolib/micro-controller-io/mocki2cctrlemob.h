#ifndef MOCKI2CCTRLEMOB_H
#define MOCKI2CCTRLEMOB_H

#include "abstractalli2ccontrollers.h"

class MockI2cCtrlEMOB : public AbstractI2cCtrlEMOB
{
public:
    ZeraMControllerIo::atmelRM sendPushbuttonPress(QString channelName) override;
    ZeraMControllerIo::atmelRM readEmobLockState(quint8 &status, QString channelName) override;
};

#endif // MOCKI2CCTRLEMOB_H
