#ifndef MOCKI2CCTRLDEVICEIDENTIFICATIONDATA_H
#define MOCKI2CCTRLDEVICEIDENTIFICATIONDATA_H

#include "abstracti2ccontrollers.h"

class MockI2cCtrlDeviceIdentificationData : public AbstractI2cCtrlDeviceIdentificationData
{
public:
    MockI2cCtrlDeviceIdentificationData(QString &deviceName, QString &serialNumber, QString &LCAVersion, QString &writablePcbVersion);
    ZeraMControllerIo::atmelRM readDeviceName(QString& answer) override;
    ZeraMControllerIo::atmelRM readSerialNumber(QString& answer) override;
    ZeraMControllerIo::atmelRM writeSerialNumber(QString &sNumber) override;
    ZeraMControllerIo::atmelRM readLCAVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM writePCBVersion(QString& sVersion) override;
private:
    QString &m_deviceName;
    QString &m_serialNumber;
    QString &m_LCAVersion;
    QString &m_writablePcbVersion;
};

#endif // MOCKI2CCTRLDEVICEIDENTIFICATIONDATA_H
