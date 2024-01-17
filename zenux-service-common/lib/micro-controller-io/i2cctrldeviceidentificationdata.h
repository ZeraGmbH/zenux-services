#ifndef I2CCTRLDEVICEIDENTIFICATIONDATA_H
#define I2CCTRLDEVICEIDENTIFICATIONDATA_H

#include "abstracti2ccontrollers.h"

class I2cCtrlDeviceIdentificationData : public AbstractI2cCtrlDeviceIdentificationData
{
public:
    I2cCtrlDeviceIdentificationData(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIo::atmelRM readDeviceName(QString& answer) override;
    ZeraMControllerIo::atmelRM readSerialNumber(QString& answer) override;
    ZeraMControllerIo::atmelRM writeSerialNumber(QString &sNumber) override;
    ZeraMControllerIo::atmelRM readLCAVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM writePCBVersion(QString& sVersion) override; // only relais controller for now
private:
    ZeraMControllerIo m_ctrlIo;
};

#endif // I2CCTRLDEVICEIDENTIFICATIONDATA_H
