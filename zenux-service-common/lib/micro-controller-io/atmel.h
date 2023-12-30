#ifndef ATMEL_H
#define ATMEL_H

#include "atmelpermissiontemplate.h"
#include "atmelcommon.h"
#include "atmelcommonversions.h"
#include <zeramcontrollerio.h>
#include <QString>

class Atmel : public AtmelCommon, public AtmelCommonVersions, public AtmelPermissionTemplate
{
public:
    static void setInstanceParams(QString devnode, quint8 adr, quint8 debuglevel);
    static Atmel &getInstance();

    atmelRM readSerialNumber(QString& answer);
    atmelRM writeSerialNumber(QString &sNumber);
    atmelRM readDeviceName(QString& answer);
    atmelRM readPCBVersion(QString& answer) override;
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

    /* mmode COM5003:
    enum enMeasurementModes
    {
        MEAS_MODE_NORMAL = 0,
        MEAS_MODE_0V_ABGL,
        MEAS_MODE_1V_REF,

        MEAS_MODE_COUNT
    };
     * see https://github.com/ZeraGmbH/atmel-com5003/blob/1664607ebdd8938c479f182c48bac8afe901918c/Control_Relais.h#L17
     */
    atmelRM setMeasMode(quint8 mmode);
    atmelRM readMeasMode(quint8& mmode);

    atmelRM setPLLChannel(quint8 chn);
    atmelRM readPLLChannel(quint8& chn);
private:
    Atmel(QString devnode, quint8 adr, quint8 debuglevel);
    static QString m_devnode;
    static quint8 m_adr;
    static quint8 m_debuglevel;
};

#endif // ATMEL_H
