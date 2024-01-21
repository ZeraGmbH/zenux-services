#ifndef DEMOI2CCTRLACCU_H
#define DEMOI2CCTRLACCU_H

#include "abstractalli2ccontrollers.h"

class DemoI2cCtrlAccu : public AbstractI2cCtrlAccu
{
public:
    ZeraMControllerIo::atmelRM readAccuStatus(quint8& stat) override;
    ZeraMControllerIo::atmelRM readAccuStateOfCharge(quint8& charge) override;
    ZeraMControllerIo::atmelRM enableTestMode(qint32 testBits) override;
};

#endif // DEMOI2CCTRLACCU_H
