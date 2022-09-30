#ifndef ATMEL_H
#define ATMEL_H

#include <QString>
#include <intelhexfileio.h>
#include <crcutils.h>
#include <protocol_zera_bootloader.h>
#include <protocol_zera_hard.h>
#include <zera_mcontroller_base.h>
#include "atmelbaseinterface.h"

class cATMEL : public ZeraMcontrollerBase, public AtmelBaseInterface
{
public:
    cATMEL(QString devnode, quint8 adr, quint8 debuglevel);

    atmelRM readSerialNumber(QString& answer);
    atmelRM writeSerialNumber(QString &sNumber);
    atmelRM readDeviceName(QString& answer);
    atmelRM readPCBVersion(QString& answer);
    atmelRM writePCBVersion(QString& sVersion);
    atmelRM readCTRLVersion(QString& answer) override;
    atmelRM readLCAVersion(QString& answer);
    atmelRM startBootLoader();
    atmelRM readChannelStatus(quint8 channel, quint8& stat);
    atmelRM readCriticalStatus(quint16& stat);
    atmelRM resetCriticalStatus(quint16 stat);
    atmelRM readClampStatus(quint16& stat);

    atmelRM writeIntMask(quint16 mask);
    atmelRM readIntMask(quint16& mask);

    atmelRM readRange(quint8 channel, quint8& range);
    atmelRM setRange(quint8 channel, quint8 range);
    atmelRM getEEPROMAccessEnable(bool& enable);
    atmelRM readSamplingRange(quint8& srange);
    atmelRM setSamplingRange(quint8 );
    atmelRM setMeasMode(quint8 mmode);
    atmelRM readMeasMode(quint8& mmode);
    atmelRM setPLLChannel(quint8 chn);
    atmelRM readPLLChannel(quint8& chn);
};

extern cATMEL* pAtmel;

#endif // ATMEL_H
