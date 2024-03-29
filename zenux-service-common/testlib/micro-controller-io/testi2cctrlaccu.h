#ifndef TESTI2CCTRLACCU_H
#define TESTI2CCTRLACCU_H

#include "abstractalli2ccontrollers.h"

class TestI2cCtrlAccu : public AbstractI2cCtrlAccu
{
public:
    ZeraMControllerIo::atmelRM readAccuStatus(quint8& stat) override;
    ZeraMControllerIo::atmelRM readAccuStateOfCharge(quint8& charge) override;
};

#endif // TESTI2CCTRLACCU_H
