#ifndef I2CCTRLDEVICEIDENTIFICATIONDATA_H
#define I2CCTRLDEVICEIDENTIFICATIONDATA_H

#include "abstractalli2ccontrollers.h"

class I2cCtrlDeviceIdent : public AbstractI2cCtrlDeviceIdent
{
public:
    I2cCtrlDeviceIdent(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIo::atmelRM readDeviceName(QString& answer) override;
    ZeraMControllerIo::atmelRM readSerialNumber(QString& answer) override;
    ZeraMControllerIo::atmelRM writeSerialNumber(QString &sNumber) override;
    ZeraMControllerIo::atmelRM readLCAVersion(QString& answer) override;
private:
    ZeraMControllerIo m_ctrlIo;
};

#endif // I2CCTRLDEVICEIDENTIFICATIONDATA_H
