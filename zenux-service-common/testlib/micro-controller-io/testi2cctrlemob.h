#ifndef TESTI2CCTRLEMOB_H
#define TESTI2CCTRLEMOB_H

#include "abstractalli2ccontrollers.h"

class TestI2cCtrlEMOB : public AbstractI2cCtrlEMOB
{
public:
    TestI2cCtrlEMOB(int ctrlMuxChannnel);
    ZeraMControllerIo::atmelRM sendPushbuttonPress() override;
    ZeraMControllerIo::atmelRM readEmobLockState(quint8 &status) override;
    ZeraMControllerIo::atmelRM readEmobInstrumentSubType(QString& answer) override;
private:
    int m_ctrlMuxChannnel;
};

#endif // TESTI2CCTRLEMOB_H
