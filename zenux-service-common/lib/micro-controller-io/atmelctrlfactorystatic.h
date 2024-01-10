#ifndef ATMELCTRLFACTORYSTATIC_H
#define ATMELCTRLFACTORYSTATIC_H

#include "atmelemobctrl.h"
#include "atmelwatcherinterface.h"

class AtmelCtrlFactoryStatic // TODO: Split up
{
public:
    static AtmelCommonVersionsPtr getEmobCtrl(QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 muxChannel, quint8 debuglevel);
    static AtmelWatcherInterfacePtr createAtmelWatcher(QString devnode);
protected:
    static std::function<AtmelCommonVersionsPtr(QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 ctrlChannelForMux, quint8 debuglevel)> m_emobCreateFunction;
    static std::function<AtmelWatcherInterfacePtr(QString devnode)> m_atmelWatcherCreateFunction;
};

#endif // ATMELCTRLFACTORYSTATIC_H
