#ifndef ATMELCTRLRELAIS_H
#define ATMELCTRLRELAIS_H

#include "abstracti2ccontrollers.h"
#include <zeramcontrollerio.h>
#include <QString>

class AtmelCtrlRelais : public ZeraMControllerIo,
                        public AbstractI2cCtrlCommonVersions
{
public:
    AtmelCtrlRelais(QString devnode, quint8 adr, quint8 debuglevel);
    atmelRM readPCBVersion(QString& answer) override;
    atmelRM readCTRLVersion(QString& answer) override;
    atmelRM readChannelStatus(quint8 channel, quint8& stat);

private:
    static QString m_devnode;
    static quint8 m_adr;
    static quint8 m_debuglevel;
};

#endif // ATMELCTRLRELAIS_H
