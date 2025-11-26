#ifndef TESTI2CCTRLEMOB_H
#define TESTI2CCTRLEMOB_H

#include "abstractalli2ccontrollers.h"

class TestI2cCtrlEMOB : public AbstractI2cCtrlEMOB
{
public:
    TestI2cCtrlEMOB(qint8 muxChannel);
    ZeraMControllerIo::atmelRM sendPushbuttonPress() override;
    ZeraMControllerIo::atmelRM readEmobLockState(quint8 &status) override;
    ZeraMControllerIo::atmelRM readEmobInstrumentSubType(QString& answer) override;
    ZeraMControllerIo::atmelRM readEmobErrorStatus(quint8& err) override;
    ZeraMControllerIo::atmelRM clearErrorStatus() override;
private:
    qint8 m_muxChannel;
};

#endif // TESTI2CCTRLEMOB_H
