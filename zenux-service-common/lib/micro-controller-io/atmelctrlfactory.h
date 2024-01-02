#ifndef ATMELCTRLFACTORY_H
#define ATMELCTRLFACTORY_H

#include "atmelemobctrl.h"
#include "atmelwatcherinterface.h"

class AtmelCtrlFactory // TODO: Split up
{
public:
    static AtmelCommonVersionsPtr createEmobCtrl(QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 muxChannel, quint8 debuglevel);
    static AtmelWatcherInterfacePtr createAtmelWatcher(QString devnode);
protected:
    static std::function<AtmelCommonVersionsPtr(QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 ctrlChannelForMux, quint8 debuglevel)> m_emobCreateFunction;
    static std::function<AtmelWatcherInterfacePtr(QString devnode)> m_atmelWatcherCreateFunction;
};

#endif // ATMELCTRLFACTORY_H
