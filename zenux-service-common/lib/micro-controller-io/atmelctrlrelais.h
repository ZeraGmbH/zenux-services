#ifndef ATMELCTRLRELAIS_H
#define ATMELCTRLRELAIS_H

#include "atmelcommon.h"
#include <zeramcontrollerio.h>
#include <QString>

class AtmelCtrlRelais : public AtmelCommon,
                        public AtmelCommonVersions,
                        public AtmelDeviceIdentificationData,
                        public AtmelPermissionTemplate,
                        public AtmelRanges
{
public:
    AtmelCtrlRelais(QString devnode, quint8 adr, quint8 debuglevel);
    atmelRM readSerialNumber(QString& answer) override;
    atmelRM writeSerialNumber(QString &sNumber) override;
    atmelRM readDeviceName(QString& answer) override;
    atmelRM readPCBVersion(QString& answer) override;
    atmelRM writePCBVersion(QString& sVersion) override;
    atmelRM readCTRLVersion(QString& answer) override;
    atmelRM readLCAVersion(QString& answer) override;
    atmelRM startBootLoader();
    atmelRM readChannelStatus(quint8 channel, quint8& stat);
    atmelRM readClampStatus(quint16& stat);

    atmelRM readRange(quint8 channel, quint8& range) override;
    atmelRM setRange(quint8 channel, quint8 range) override;
    atmelRM getEEPROMAccessEnable(bool& enable) override;

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
    static QString m_devnode;
    static quint8 m_adr;
    static quint8 m_debuglevel;
};

#endif // ATMELCTRLRELAIS_H
