#ifndef ATMELCTRLRELAIS_H
#define ATMELCTRLRELAIS_H

#include "abstracti2ccontrollers.h"
#include <zeramcontrollerio.h>
#include <QString>

class AtmelCtrlRelais : public ZeraMControllerIo,
                        public AbstractI2cCtrlCommonVersions,
                        public AbstractI2cCtrlMMode,
                        public AbstractI2cCtrlPll
{
public:
    AtmelCtrlRelais(QString devnode, quint8 adr, quint8 debuglevel);
    atmelRM readPCBVersion(QString& answer) override;
    atmelRM readCTRLVersion(QString& answer) override;
    atmelRM readChannelStatus(quint8 channel, quint8& stat);

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
    atmelRM setMeasMode(quint8 mmode) override;
    atmelRM readMeasMode(quint8& mmode) override;

    atmelRM setPLLChannel(quint8 chn) override;
    atmelRM readPLLChannel(quint8& chn) override;
private:
    static QString m_devnode;
    static quint8 m_adr;
    static quint8 m_debuglevel;
};

#endif // ATMELCTRLRELAIS_H
