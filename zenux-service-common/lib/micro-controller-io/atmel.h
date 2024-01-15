#ifndef ATMEL_H
#define ATMEL_H

#include "atmelcommon.h"
#include <zeramcontrollerio.h>
#include <QString>

class Atmel : public AtmelCommon
{
public:
    static void setInstanceParams(QString devnode, quint8 adr, quint8 debuglevel);
    static Atmel &getInstance();

    atmelRM startBootLoader();
    atmelRM readChannelStatus(quint8 channel, quint8& stat);
    atmelRM readClampStatus(quint16& stat);

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
    atmelRM setPLLChannel(quint8 chn);
    atmelRM readPLLChannel(quint8& chn);
private:
    Atmel(QString devnode, quint8 adr, quint8 debuglevel);
    static QString m_devnode;
    static quint8 m_adr;
    static quint8 m_debuglevel;
};

#endif // ATMEL_H
