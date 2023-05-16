#ifndef ATMELCTRLFACTORY_H
#define ATMELCTRLFACTORY_H

#include "atmelemobctrl.h"
#include <memory>

class AtmelCtrlFactory
{
public:
    static std::shared_ptr<AtmelCommonVersions> createEmobCtrl(QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 muxChannel, quint8 debuglevel);
protected:
    static std::function<AtmelCommonVersionsPtr(QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 ctrlChannelForMux, quint8 debuglevel)> m_emobCreateFunction;
};

#endif // ATMELCTRLFACTORY_H
