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
    virtual atmelRM readAccumulatorStatus(quint16& stat);
};

extern cATMELSysCtrl* pAtmelSys;

#endif // ATMELATMELSYS_H
