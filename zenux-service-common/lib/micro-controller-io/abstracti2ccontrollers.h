#ifndef ABSTRACTCONTROLLERS_H
#define ABSTRACTCONTROLLERS_H

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
typedef std::unique_ptr<AbstractI2cCtrlCriticalStatus> AtmelCriticalStatusPtr;

class AbstractI2cCtrlCommonVersions
{
public:
    virtual ~AbstractI2cCtrlCommonVersions() = default;
    virtual ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) = 0;
    virtual ZeraMControllerIo::atmelRM readPCBVersion(QString& answer) = 0;
};
typedef std::shared_ptr<AbstractI2cCtrlCommonVersions> AtmelCommonVersionsPtrS;
typedef std::unique_ptr<AbstractI2cCtrlCommonVersions> AtmelCommonVersionsPtrU;

class AbstractI2cCtrlDeviceIdentificationData
{
public:
    virtual ~AbstractI2cCtrlDeviceIdentificationData() = default;
    virtual ZeraMControllerIo::atmelRM readDeviceName(QString& answer) = 0;
    virtual ZeraMControllerIo::atmelRM readSerialNumber(QString& answer) = 0;
    virtual ZeraMControllerIo::atmelRM writeSerialNumber(QString &sNumber) = 0;
    virtual ZeraMControllerIo::atmelRM readLCAVersion(QString& answer) = 0;
    virtual ZeraMControllerIo::atmelRM writePCBVersion(QString& sVersion) = 0; // only relais controller for now
};
typedef std::unique_ptr<AbstractI2cCtrlDeviceIdentificationData> AtmelDeviceIdentificationDataU;


class AbstractI2cCtrlEepromPermission
{
public:
    virtual ~AbstractI2cCtrlEepromPermission() = default;
    virtual ZeraMControllerIo::atmelRM getEEPROMAccessEnable(bool &enable) = 0;
    bool /*success*/ hasPermission(bool &allow);
};
typedef std::unique_ptr<AbstractI2cCtrlEepromPermission> AtmelPermissionTemplatePtrU;


class AbstractI2cCtrlRanges
{
public:
    virtual ~AbstractI2cCtrlRanges() = default;
    virtual ZeraMControllerIo::atmelRM readRange(quint8 channel, quint8& range) = 0;
    virtual ZeraMControllerIo::atmelRM setRange(quint8 channel, quint8 range) = 0;
};
typedef std::unique_ptr<AbstractI2cCtrlRanges> AtmelRangesPtrU;

class AbstractI2cCtrlMMode
{
public:
    virtual ~AbstractI2cCtrlMMode() = default;
    virtual ZeraMControllerIo::atmelRM setMeasMode(quint8 mmode) = 0;
    virtual ZeraMControllerIo::atmelRM readMeasMode(quint8& mmode) = 0;
};
typedef std::unique_ptr<AbstractI2cCtrlMMode> AtmelMModesPtrU;


class AbstractI2cCtrlPll
{
public:
    virtual ~AbstractI2cCtrlPll() = default;
    virtual ZeraMControllerIo::atmelRM setPLLChannel(quint8 chn) = 0;
    virtual ZeraMControllerIo::atmelRM readPLLChannel(quint8& chn) = 0;
};
typedef std::unique_ptr<AbstractI2cCtrlPll> AtmelPllPtrU;

class AbstractI2cCtrlAccumulator
{
public:
    virtual ~AbstractI2cCtrlAccumulator() = default;
    virtual ZeraMControllerIo::atmelRM readAccumulatorStatus(quint8& stat) = 0;
    virtual ZeraMControllerIo::atmelRM readAccuStateOfCharge(quint8& charge) = 0;
    // This should go soon!!!
    virtual ZeraMControllerIo::atmelRM enableTestMode(qint32 testBits) = 0;
};

typedef std::unique_ptr<AbstractI2cCtrlAccumulator> AtmelAccumulatorHandlerPtrU;

#endif // ABSTRACTCONTROLLERS_H
