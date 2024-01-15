#ifndef ATMELCOMMON_H
#define ATMELCOMMON_H

#include <zeramcontrollerio.h>
#include <memory>

class AtmelCommon : public ZeraMControllerIo
{
public:
    AtmelCommon(QString devnode, quint8 adr, quint8 debuglevel);
    virtual ~AtmelCommon() = default;

    atmelRM writeIntMask(quint16 mask);
    atmelRM readIntMask(quint16& mask);
    atmelRM readCriticalStatus(quint16& stat);
    atmelRM resetCriticalStatus(quint16 stat);
};

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
    virtual AtmelCommon::atmelRM readDeviceName(QString& answer) = 0;
    virtual AtmelCommon::atmelRM readSerialNumber(QString& answer) = 0;
    virtual AtmelCommon::atmelRM writeSerialNumber(QString &sNumber) = 0;
    virtual AtmelCommon::atmelRM readLCAVersion(QString& answer) = 0;
};


class AtmelPermissionTemplate
{
public:
    virtual ~AtmelPermissionTemplate() = default;
    virtual ZeraMControllerIo::atmelRM getEEPROMAccessEnable(bool &enable) = 0;
    bool /*success*/ hasPermission(bool &allow);
};
typedef std::unique_ptr<AtmelPermissionTemplate> AtmelPermissionTemplatePtrU;


class AtmelAccumulatorHandler
{
public:
    virtual ZeraMControllerIo::atmelRM readAccumulatorStatus(quint8& stat) = 0;
    virtual ZeraMControllerIo::atmelRM readAccuStateOfCharge(quint8& charge) = 0;
    // This should go soon!!!
    virtual ZeraMControllerIo::atmelRM enableTestMode(qint32 testBits) = 0;
};

typedef std::unique_ptr<AtmelAccumulatorHandler> AtmelAccumulatorHandlerPtrU;


#endif // ATMELCOMMON_H
