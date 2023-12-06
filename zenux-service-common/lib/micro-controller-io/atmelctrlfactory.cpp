#include "atmelctrlfactory.h"
#include "atmelwatcher.h"

std::function<AtmelCommonVersionsPtr(QString, quint8, quint8, quint8, quint8)> AtmelCtrlFactory::m_emobCreateFunction =
    [](QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 ctrlChannel, quint8 debuglevel) {
    ZeraMcontrollerIoPtr i2cCtrl = std::make_shared<ZeraMControllerIo>(devnode, adrCtrl, debuglevel);
    return std::make_shared<AtmelEmobCtrl>(i2cCtrl, devnode, adrMux, ctrlChannel);
};

std::function<AtmelWatcherInterfacePrt(QString devnode)> AtmelCtrlFactory::m_atmelWatcherCreateFunction =
    [](QString devnode) {
        return std::make_unique<cAtmelWatcher>(devnode);
};

AtmelCommonVersionsPtr AtmelCtrlFactory::createEmobCtrl(QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 muxChannel, quint8 debuglevel)
{
    return m_emobCreateFunction(devnode, adrCtrl, adrMux, muxChannel, debuglevel);
}

AtmelWatcherInterfacePrt AtmelCtrlFactory::createAtmelWatcher(QString devnode)
{
    return m_atmelWatcherCreateFunction(devnode);
}
