#ifndef I2CCTRLCRITICALSTATUS_H
#define I2CCTRLCRITICALSTATUS_H

#include "abstractalli2ccontrollers.h"

class I2cCtrlCriticalStatus : public AbstractI2cCtrlCriticalStatus
{
public:
    I2cCtrlCriticalStatus(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIo::atmelRM writeIntMask(quint16 mask) override;
    ZeraMControllerIo::atmelRM readIntMask(quint16& mask) override;
    ZeraMControllerIo::atmelRM readCriticalStatus(quint16& stat) override;
    ZeraMControllerIo::atmelRM resetCriticalStatus(quint16 stat) override;
private:
    ZeraMControllerIo m_ctrlIo;
};

#endif // I2CCTRLCRITICALSTATUS_H
