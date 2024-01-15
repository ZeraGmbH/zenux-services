#ifndef MOCKATMELDEVICEIDENTIFICATIONDATA_H
#define MOCKATMELDEVICEIDENTIFICATIONDATA_H

#include "atmelcommon.h"

class MockAtmelDeviceIdentificationData : public AtmelDeviceIdentificationData
{
public:
    MockAtmelDeviceIdentificationData(QString &deviceName, QString &serialNumber, QString &LCAVersion, QString &writablePcbVersion);
    AtmelCommon::atmelRM readDeviceName(QString& answer) override;
    AtmelCommon::atmelRM readSerialNumber(QString& answer) override;
    AtmelCommon::atmelRM writeSerialNumber(QString &sNumber) override;
    AtmelCommon::atmelRM readLCAVersion(QString& answer) override;
    AtmelCommon::atmelRM writePCBVersion(QString& sVersion) override;
private:
    QString &m_deviceName;
    QString &m_serialNumber;
    QString &m_LCAVersion;
    QString &m_writablePcbVersion;
};

#endif // MOCKATMELDEVICEIDENTIFICATIONDATA_H
