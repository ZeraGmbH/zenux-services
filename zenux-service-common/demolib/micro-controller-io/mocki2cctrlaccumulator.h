#ifndef MOCKI2CCTRLACCUMULATOR_H
#define MOCKI2CCTRLACCUMULATOR_H

#include "abstracti2ccontrollers.h"

class MockI2cCtrlAccumulator : public AbstractI2cCtrlAccumulator
{
public:
    ZeraMControllerIo::atmelRM readAccuStatus(quint8& stat) override;
    ZeraMControllerIo::atmelRM readAccuStateOfCharge(quint8& charge) override;
    ZeraMControllerIo::atmelRM enableTestMode(qint32 testBits) override;
};

#endif // MOCKI2CCTRLACCUMULATOR_H
