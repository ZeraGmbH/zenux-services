#ifndef MOCKI2CCTRLDEVICEIDENTIFICATIONDATA_H
#define MOCKI2CCTRLDEVICEIDENTIFICATIONDATA_H

#include "abstracti2ccontrollers.h"

class MockI2cCtrlDeviceIdent : public AbstractI2cCtrlDeviceIdent
{
public:
    MockI2cCtrlDeviceIdent(QString &deviceName, QString &serialNumber, QString &LCAVersion, QString &writablePcbVersion);
    ZeraMControllerIo::atmelRM readDeviceName(QString& answer) override;
    ZeraMControllerIo::atmelRM readSerialNumber(QString& answer) override;
    ZeraMControllerIo::atmelRM writeSerialNumber(QString &sNumber) override;
    ZeraMControllerIo::atmelRM readLCAVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM writePCBVersion(QString& sVersion) override;
private:
    QString &m_deviceName;
    QString &m_serialNumber;
    QString &m_FPGAVersion;
    QString &m_writablePcbVersion;
};

#endif // MOCKI2CCTRLDEVICEIDENTIFICATIONDATA_H
