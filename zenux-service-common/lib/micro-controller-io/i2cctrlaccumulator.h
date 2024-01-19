#ifndef I2CCTRLACCUMULATOR_H
#define I2CCTRLACCUMULATOR_H

#include "abstracti2ccontrollers.h"

class I2cCtrlAccumulator : public AbstractI2cCtrlAccumulator
{
public:
    I2cCtrlAccumulator(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIo::atmelRM readAccuStatus(quint8& stat) override;
    ZeraMControllerIo::atmelRM readAccuStateOfCharge(quint8& charge) override;
    // this one will go soon
    ZeraMControllerIo::atmelRM enableTestMode(qint32 testBits) override;
private:
    ZeraMControllerIo m_ctrlIo;
};

#endif // I2CCTRLACCUMULATOR_H
