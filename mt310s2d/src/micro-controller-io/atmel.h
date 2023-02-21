#ifndef ATMEL_H
#define ATMEL_H

#include "atmelpermissiontemplate.h"
#include "atmelcommon.h"
#include <zera_mcontroller_base.h>
#include <QString>

class cATMEL : public AtmelCommon, public AtmelPermissionTemplate
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
    atmelRM readClampStatus(quint16& stat);

    atmelRM readRange(quint8 channel, quint8& range);
    atmelRM setRange(quint8 channel, quint8 range);
    atmelRM getEEPROMAccessEnable(bool& enable) override;
    atmelRM readSamplingRange(quint8& srange);
    atmelRM setSamplingRange(quint8 );
    atmelRM setMeasMode(quint8 mmode);
    atmelRM readMeasMode(quint8& mmode);
    atmelRM setPLLChannel(quint8 chn);
    atmelRM readPLLChannel(quint8& chn);
};

extern cATMEL* pAtmel;

#endif // ATMEL_H
