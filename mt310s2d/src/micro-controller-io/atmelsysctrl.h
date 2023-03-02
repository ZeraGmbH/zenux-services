#ifndef ATMELATMELSYS_H
#define ATMELATMELSYS_H

#include <atmelcommon.h>
#include <intelhexfileio.h>
#include <zera_mcontroller_base.h>
#include <QString>

class cATMELSysCtrl : public AtmelCommon
{
public:
    cATMELSysCtrl(QString devnode, quint8 adr, quint8 debuglevel);

    atmelRM readCTRLVersion(QString& answer) override;
    virtual atmelRM readAccumulatorStatus(quint8& stat);
    virtual atmelRM readAccumulatorSoc(quint8& charge);
    atmelRM enableTestMode(qint32 testBits);
};

extern cATMELSysCtrl* pAtmelSys;

#endif // ATMELATMELSYS_H
