#ifndef ABSTRACTCONTROLLERS_H
#define ABSTRACTCONTROLLERS_H

#include <zeramcontrollerio.h>
#include <memory>

class AtmelCriticalStatus
{
public:
    virtual ~AtmelCriticalStatus()  = default;
    virtual ZeraMControllerIo::atmelRM writeIntMask(quint16 mask) = 0;
    virtual ZeraMControllerIo::atmelRM readIntMask(quint16& mask) = 0;
    virtual ZeraMControllerIo::atmelRM readCriticalStatus(quint16& stat) = 0;
    virtual ZeraMControllerIo::atmelRM resetCriticalStatus(quint16 stat) = 0;
};
typedef std::unique_ptr<AtmelCriticalStatus> AtmelCriticalStatusPtr;

class AtmelCommonVersions
{
public:
    virtual ~AtmelCommonVersions() = default;
    virtual ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) = 0;
    virtual ZeraMControllerIo::atmelRM readPCBVersion(QString& answer) = 0;
};
typedef std::shared_ptr<AtmelCommonVersions> AtmelCommonVersionsPtrS;
typedef std::unique_ptr<AtmelCommonVersions> AtmelCommonVersionsPtrU;

class AtmelDeviceIdentificationData
{
public:
    virtual ~AtmelDeviceIdentificationData() = default;
    virtual ZeraMControllerIo::atmelRM readDeviceName(QString& answer) = 0;
    virtual ZeraMControllerIo::atmelRM readSerialNumber(QString& answer) = 0;
    virtual ZeraMControllerIo::atmelRM writeSerialNumber(QString &sNumber) = 0;
    virtual ZeraMControllerIo::atmelRM readLCAVersion(QString& answer) = 0;
    virtual ZeraMControllerIo::atmelRM writePCBVersion(QString& sVersion) = 0; // only relais controller for now
};
typedef std::unique_ptr<AtmelDeviceIdentificationData> AtmelDeviceIdentificationDataU;


class AtmelPermissionTemplate
{
public:
    virtual ~AtmelPermissionTemplate() = default;
    virtual ZeraMControllerIo::atmelRM getEEPROMAccessEnable(bool &enable) = 0;
    bool /*success*/ hasPermission(bool &allow);
};
typedef std::unique_ptr<AtmelPermissionTemplate> AtmelPermissionTemplatePtrU;


class AtmelRanges
{
public:
    virtual ~AtmelRanges() = default;
    virtual ZeraMControllerIo::atmelRM readRange(quint8 channel, quint8& range) = 0;
    virtual ZeraMControllerIo::atmelRM setRange(quint8 channel, quint8 range) = 0;
};
typedef std::unique_ptr<AtmelRanges> AtmelRangesPtrU;

class AtmelMModes
{
public:
    virtual ~AtmelMModes() = default;
    virtual ZeraMControllerIo::atmelRM setMeasMode(quint8 mmode) = 0;
    virtual ZeraMControllerIo::atmelRM readMeasMode(quint8& mmode) = 0;
};
typedef std::unique_ptr<AtmelMModes> AtmelMModesPtrU;


class AtmelPll
{
public:
    virtual ~AtmelPll() = default;
    virtual ZeraMControllerIo::atmelRM setPLLChannel(quint8 chn) = 0;
    virtual ZeraMControllerIo::atmelRM readPLLChannel(quint8& chn) = 0;
};
typedef std::unique_ptr<AtmelPll> AtmelPllPtrU;

class AtmelAccumulatorHandler
{
public:
    virtual ~AtmelAccumulatorHandler() = default;
    virtual ZeraMControllerIo::atmelRM readAccumulatorStatus(quint8& stat) = 0;
    virtual ZeraMControllerIo::atmelRM readAccuStateOfCharge(quint8& charge) = 0;
    // This should go soon!!!
    virtual ZeraMControllerIo::atmelRM enableTestMode(qint32 testBits) = 0;
};

typedef std::unique_ptr<AtmelAccumulatorHandler> AtmelAccumulatorHandlerPtrU;

#endif // ABSTRACTCONTROLLERS_H
