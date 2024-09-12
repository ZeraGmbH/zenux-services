#ifndef I2CCTRLACCU_H
#define I2CCTRLACCU_H

#include "abstractalli2ccontrollers.h"

class I2cCtrlAccu : public AbstractI2cCtrlAccu
{
public:
    I2cCtrlAccu(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIo::atmelRM readAccuStatus(quint8& stat) override;
    ZeraMControllerIo::atmelRM readAccuStateOfCharge(quint8& charge) override;
private:
    ZeraMControllerIo m_ctrlIo; // we pass through systemcontroller!!
};

#endif // I2CCTRLACCU_H
