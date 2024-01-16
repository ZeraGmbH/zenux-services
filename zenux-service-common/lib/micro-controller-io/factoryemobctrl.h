#ifndef FACTORYEMOBCTRL_H
#define FACTORYEMOBCTRL_H

#include "atmelctrlemob.h"

class FactoryEmobCtrl // TODO: Rename or make better
{
public:
    static AtmelCommonVersionsPtrS getEmobCtrl(QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 muxChannel, quint8 debuglevel);
protected:
    static std::function<AtmelCommonVersionsPtrS(QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 ctrlChannelForMux, quint8 debuglevel)> m_emobCreateFunction;
};

#endif // FACTORYEMOBCTRL_H