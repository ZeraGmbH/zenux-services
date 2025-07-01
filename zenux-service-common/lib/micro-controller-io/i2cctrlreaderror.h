#ifndef I2CCTRLREADERROR_H
#define I2CCTRLREADERROR_H

#include "abstractalli2ccontrollers.h"

class I2cCtrlReadError : public AbstractI2cCtrlReadError
{
public:
    I2cCtrlReadError(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIo::atmelRM readError(QString& answer) override;
private:
    ZeraMControllerIo m_ctrlIo;
};


#endif // I2CCTRLREADERROR_H
