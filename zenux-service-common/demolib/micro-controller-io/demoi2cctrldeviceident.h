#ifndef DEMOI2CCTRLDEVICEIDENTIFICATIONDATA_H
#define DEMOI2CCTRLDEVICEIDENTIFICATIONDATA_H

#include "abstractalli2ccontrollers.h"

class DemoI2cCtrlDeviceIdent : public AbstractI2cCtrlDeviceIdent
{
public:
    DemoI2cCtrlDeviceIdent(QString &serialNumber, QString &LCAVersion, QString &writablePcbVersion);
    ZeraMControllerIo::atmelRM readDeviceName(QString& answer) override;
    ZeraMControllerIo::atmelRM readSerialNumber(QString& answer) override;
    ZeraMControllerIo::atmelRM writeSerialNumber(QString &sNumber) override;
    ZeraMControllerIo::atmelRM readLCAVersion(QString& answer) override;
private:
    QString &m_serialNumber;
    QString &m_FPGAVersion;
    QString &m_writablePcbVersion;
};

#endif // DEMOI2CCTRLDEVICEIDENTIFICATIONDATA_H
