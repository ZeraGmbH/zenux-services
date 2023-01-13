#ifndef ATMELATMELSYS_H
#define ATMELATMELSYS_H

#include "atmelbaseinterface.h"
#include <intelhexfileio.h>
#include <zera_mcontroller_base.h>
#include <QString>

class cATMELSysCtrl : public ZeraMcontrollerBase, public AtmelBaseInterface
{
public:
    cATMELSysCtrl(QString devnode, quint8 adr, quint8 debuglevel);

    atmelRM readCTRLVersion(QString& answer) override;
};

extern cATMELSysCtrl* pAtmelSys;

#endif // ATMELATMELSYS_H
