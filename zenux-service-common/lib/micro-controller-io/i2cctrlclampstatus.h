#ifndef I2CCTRLCLAMPSTATUS_H
#define I2CCTRLCLAMPSTATUS_H

#include "abstracti2ccontrollers.h"

class I2cCtrlClampStatus : public AbstractI2cCtrlClampStatus
{
public:
    I2cCtrlClampStatus(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIo::atmelRM readClampStatus(quint16& stat) override;
private:
    ZeraMControllerIo m_ctrlIo;
};

#endif // I2CCTRLCLAMPSTATUS_H
