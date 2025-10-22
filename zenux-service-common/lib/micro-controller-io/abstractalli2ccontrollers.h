#ifndef ABSTRACTCONTROLLERS_H
#define ABSTRACTCONTROLLERS_H

#include "abstractzeramcontrollerdelayed.h"
#include <zeramcontrollerio.h>
#include <memory>

class AbstractI2cCtrlCriticalStatus
{
public:
    virtual ~AbstractI2cCtrlCriticalStatus()  = default;
    virtual ZeraMControllerIo::atmelRM writeIntMask(quint16 mask) = 0;
    virtual ZeraMControllerIo::atmelRM readIntMask(quint16& mask) = 0;
    virtual ZeraMControllerIo::atmelRM readCriticalStatus(quint16& stat) = 0;
    virtual ZeraMControllerIo::atmelRM resetCriticalStatus(quint16 stat) = 0;
};
typedef std::unique_ptr<AbstractI2cCtrlCriticalStatus> I2cCtrlCriticalStatusPtr;

class AbstractI2cCtrlCommonInfo
{
public:
    virtual ~AbstractI2cCtrlCommonInfo() = default;
    virtual ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) = 0;
    virtual ZeraMControllerIo::atmelRM readPCBInfo(QString& answer) = 0;
};
typedef std::shared_ptr<AbstractI2cCtrlCommonInfo> I2cCtrlCommonInfoPtrShared;
typedef std::unique_ptr<AbstractI2cCtrlCommonInfo> I2cCtrlCommonInfoPtrUnique;

class AbstractI2cCtrlDeviceIdent
{
public:
    virtual ~AbstractI2cCtrlDeviceIdent() = default;
    virtual ZeraMControllerIo::atmelRM readDeviceName(QString& answer) = 0;
    virtual ZeraMControllerIo::atmelRM readSerialNumber(QString& answer) = 0;
    virtual ZeraMControllerIo::atmelRM writeSerialNumber(QString &sNumber) = 0;
    virtual ZeraMControllerIo::atmelRM readLCAVersion(QString& answer) = 0;
};
typedef std::unique_ptr<AbstractI2cCtrlDeviceIdent> I2cCtrlDeviceIdentPtr;


class AbstractI2cCtrlEepromPermission
{
public:
    virtual ~AbstractI2cCtrlEepromPermission() = default;
    virtual ZeraMControllerIo::atmelRM getEEPROMAccessEnable(bool &enable) = 0;
    bool /*success*/ hasPermission(bool &allow);
};
typedef std::unique_ptr<AbstractI2cCtrlEepromPermission> I2cCtrlEepromPermissionPtr;


class AbstractI2cCtrlRanges : public AbstractZeraMControllerDelayed
{
    Q_OBJECT
public:
    virtual ZeraMControllerIo::atmelRM readRange(quint8 channel, quint8& range) = 0;
    virtual ZeraMControllerIo::atmelRM setRange(quint8 channel, quint8 range) = 0;
};
typedef std::unique_ptr<AbstractI2cCtrlRanges> I2cCtrlRangesPtr;

class AbstractI2cCtrlMMode
{
public:
    virtual ~AbstractI2cCtrlMMode() = default;
    virtual ZeraMControllerIo::atmelRM setMeasMode(quint8 mmode) = 0;
    virtual ZeraMControllerIo::atmelRM readMeasMode(quint8& mmode) = 0;
};
typedef std::unique_ptr<AbstractI2cCtrlMMode> I2cCtrlMModePtr;


class AbstractI2cCtrlPll
{
public:
    virtual ~AbstractI2cCtrlPll() = default;
    virtual ZeraMControllerIo::atmelRM setPLLChannel(quint8 chn) = 0;
    virtual ZeraMControllerIo::atmelRM readPLLChannel(quint8& chn) = 0;
};
typedef std::unique_ptr<AbstractI2cCtrlPll> I2cCtrlPllPtr;

class AbstractI2cCtrlAccu
{
public:
    virtual ~AbstractI2cCtrlAccu() = default;
    virtual ZeraMControllerIo::atmelRM readAccuStatus(quint8& stat) = 0;
    virtual ZeraMControllerIo::atmelRM readAccuStateOfCharge(quint8& charge) = 0;
};
typedef std::unique_ptr<AbstractI2cCtrlAccu> I2cCtrlAccumulatorPtr;

class AbstractI2cCtrlClampStatus
{
public:
    virtual ~AbstractI2cCtrlClampStatus() = default;
    virtual ZeraMControllerIo::atmelRM readClampStatus(quint16& stat) = 0;
};
typedef std::unique_ptr<AbstractI2cCtrlClampStatus> I2cCtrlClampStatusPtr;

class AbstractI2cCtrlCpuTemperature
{
public:
    virtual ~AbstractI2cCtrlCpuTemperature() = default;
    virtual ZeraMControllerIo::atmelRM sendCpuTemperature(float &temperature) = 0;
};
typedef std::unique_ptr<AbstractI2cCtrlCpuTemperature> I2cCtrlCpuTemperaturePtr;


class AbstractI2cCtrlEMOB
{
public:
    virtual ~AbstractI2cCtrlEMOB() = default;
    virtual ZeraMControllerIo::atmelRM sendPushbuttonPress() = 0;
    virtual ZeraMControllerIo::atmelRM readEmobLockState(quint8 &status) = 0;
};
typedef std::shared_ptr<AbstractI2cCtrlEMOB> I2cCtrlEMOBPtr;


class AbstractI2cCtrlBootloader
{
public:
    virtual ~AbstractI2cCtrlBootloader() = default;
    virtual ZeraMControllerIo::atmelRM bootloaderStartProgram() = 0;
    virtual ZeraMControllerIo::atmelRM bootloaderLoadFlash(cIntelHexFileIO& ihxFIO) = 0;
};
typedef std::unique_ptr<AbstractI2cCtrlBootloader> I2cCtrlBootloaderPtr;

#endif // ABSTRACTCONTROLLERS_H
