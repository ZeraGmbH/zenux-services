#ifndef EMOBCTRLFACTORY_H
#define EMOBCTRLFACTORY_H

#include "atmelctrlemob.h"

class EmobCtrlFactory // TODO: Rename or make better
{
public:
    static AtmelCommonVersionsPtrS getEmobCtrl(QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 muxChannel, quint8 debuglevel);
protected:
    static std::function<AtmelCommonVersionsPtrS(QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 ctrlChannelForMux, quint8 debuglevel)> m_emobCreateFunction;
};

#endif // EMOBCTRLFACTORY_H
