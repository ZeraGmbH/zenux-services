#ifndef I2CCTRLRANGES_H
#define I2CCTRLRANGES_H

#include "abstracti2ccontrollers.h"

class I2cCtrlRanges : public AbstractI2cCtrlRanges
{
public:
    I2cCtrlRanges(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIo::atmelRM readRange(quint8 channel, quint8& range) override;
    ZeraMControllerIo::atmelRM setRange(quint8 channel, quint8 range) override;
private:
    ZeraMControllerIo m_ctrlIo;
};

#endif // I2CCTRLRANGES_H
