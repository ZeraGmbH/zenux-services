#ifndef ATMELATMELSYS_H
#define ATMELATMELSYS_H

#include <QString>
#include <intelhexfileio.h>
#include <crcutils.h>
#include <protocol_zera_bootloader.h>
#include <protocol_zera_hard.h>
#include <zera_mcontroller_base.h>
#include "atmelbaseinterface.h"

class cATMELSysCtrl : public ZeraMcontrollerBase, public AtmelBaseInterface
{
public:
    cATMELSysCtrl(QString devnode, quint8 adr, quint8 debuglevel);

    atmelRM readCTRLVersion(QString& answer) override;
};

extern cATMELSysCtrl* pAtmelSys;

#endif // ATMELATMELSYS_H
