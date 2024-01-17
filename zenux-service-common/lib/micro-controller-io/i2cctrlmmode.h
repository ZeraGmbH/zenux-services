#ifndef I2CCTRLMMODE_H
#define I2CCTRLMMODE_H

#include "abstracti2ccontrollers.h"

class I2cCtrlMMode : public AbstractI2cCtrlMMode
{
public:
    I2cCtrlMMode(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIo::atmelRM setMeasMode(quint8 mmode) override;
    ZeraMControllerIo::atmelRM readMeasMode(quint8& mmode) override;
private:
    ZeraMControllerIo m_ctrlIo;
};

#endif // I2CCTRLMMODE_H
