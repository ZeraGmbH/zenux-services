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

    atmelRM setPLLChannel(quint8 chn);
    atmelRM readPLLChannel(quint8& chn);
private:
    Atmel(QString devnode, quint8 adr, quint8 debuglevel);
    static QString m_devnode;
    static quint8 m_adr;
    static quint8 m_debuglevel;
};

#endif // ATMEL_H
