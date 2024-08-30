#ifndef I2CSENDTEMP_H
#define I2CSENDTEMP_H

#include "abstractalli2ccontrollers.h"

class I2cSendTemp : public AbstractI2cSendTemp
{
public:
    I2cSendTemp(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIo::atmelRM sendTemp(float temperature) override;
private:
    ZeraMControllerIo m_ctrlIo;
};

#endif // I2CSENDTEMP_H


