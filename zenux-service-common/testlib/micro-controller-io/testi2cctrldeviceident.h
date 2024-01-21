#ifndef TESTI2CCTRLDEVICEIDENTIFICATIONDATA_H
#define TESTI2CCTRLDEVICEIDENTIFICATIONDATA_H

#include "abstracti2ccontrollers.h"

class TestI2cCtrlDeviceIdent : public AbstractI2cCtrlDeviceIdent
{
public:
    TestI2cCtrlDeviceIdent(QString &serialNumber, QString &writablePcbVersion);
    ZeraMControllerIo::atmelRM readDeviceName(QString& answer) override;
    ZeraMControllerIo::atmelRM readSerialNumber(QString& answer) override;
    ZeraMControllerIo::atmelRM writeSerialNumber(QString &sNumber) override;
    ZeraMControllerIo::atmelRM readLCAVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM writePCBVersion(QString& sVersion) override;
private:
    QString &m_serialNumber;
    QString &m_writablePcbVersion;
};

#endif // TESTI2CCTRLDEVICEIDENTIFICATIONDATA_H
