#ifndef I2CCTRLCPUTEMPERATURE_H
#define I2CCTRLCPUTEMPERATURE_H

#include "abstractalli2ccontrollers.h"

class I2cCtrlCpuTemperature : public AbstractI2cCtrlCpuTemperature
{
public:
    I2cCtrlCpuTemperature(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIo::atmelRM sendCpuTemperature(float &temperature) override;

private:
    ZeraMControllerIo m_ctrlIo;
};

#endif // I2CCTRLCPUTEMPERATURE_H
