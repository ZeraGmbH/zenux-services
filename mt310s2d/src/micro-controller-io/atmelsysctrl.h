#ifndef ATMELATMELSYS_H
#define ATMELATMELSYS_H

#include <atmelcommon.h>
#include <atmelcommonversions.h>
#include <intelhexfileio.h>
#include <zeramcontrollerio.h>
#include <QString>

class cATMELSysCtrl : public AtmelCommon, public AtmelCommonVersions
{
public:
    cATMELSysCtrl(QString devnode, quint8 adr, quint8 debuglevel);

    atmelRM readCTRLVersion(QString& answer) override;
    atmelRM readPCBVersion(QString& answer) override;
    virtual atmelRM readAccumulatorStatus(quint8& stat);
    virtual atmelRM readAccumulatorSoc(quint8& charge);
    atmelRM enableTestMode(qint32 testBits);
    atmelRM sendCpuTemperatur(quint16 cpuTemp);
};

extern cATMELSysCtrl* pAtmelSys;

#endif // ATMELATMELSYS_H
