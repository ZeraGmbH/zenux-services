#ifndef DEMOI2CCTRLEMOB_H
#define DEMOI2CCTRLEMOB_H

#include "abstractalli2ccontrollers.h"

class DemoI2cCtrlEMOB : public AbstractI2cCtrlEMOB
{
public:
    ZeraMControllerIo::atmelRM sendPushbuttonPress() override;
    ZeraMControllerIo::atmelRM readEmobLockState(quint8 &status) override;
    ZeraMControllerIo::atmelRM readEmobInstrumentSubType(QString& answer) override;
    ZeraMControllerIo::atmelRM readEmobErrorStatus(quint8& err) override;
    ZeraMControllerIo::atmelRM clearErrorStatus() override;
};

#endif // DEMOI2CCTRLEMOB_H
