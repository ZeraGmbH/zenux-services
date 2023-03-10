#ifndef ATMEL_H
#define ATMEL_H

#include "atmelpermissiontemplate.h"
#include "atmelcommon.h"
#include <QString>
#include <intelhexfileio.h>
#include <crcutils.h>
#include <protocol_zera_bootloader.h>
#include <protocol_zera_hard.h>
#include <zera_mcontroller_base.h>

class cATMEL : public AtmelCommon, public AtmelPermissionTemplate
{
public:
    static void init(QString devnode, quint8 adr, quint8 debuglevel);
    static cATMEL &getInstance();

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
    atmelRM readRange(quint8 channel, quint8& range);
    atmelRM setRange(quint8 channel, quint8 range);
    atmelRM getEEPROMAccessEnable(bool& enable) override;
    atmelRM readSamplingRange(quint8& srange);
    atmelRM setSamplingRange(quint8 );
    atmelRM setMeasMode(quint8 mmode);
    atmelRM readMeasMode(quint8& mmode);
    atmelRM setPLLChannel(quint8 chn);
    atmelRM readPLLChannel(quint8& chn);
private:
    cATMEL(QString devnode, quint8 adr, quint8 debuglevel);
    static QString m_devnode;
    static quint8 m_adr;
    static quint8 m_debuglevel;
};

#endif // ATMEL_H
