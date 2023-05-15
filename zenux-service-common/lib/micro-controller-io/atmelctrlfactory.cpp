#include "atmelctrlfactory.h"

std::function<AtmelCommonVersionsPtr(QString, quint8, quint8, quint8)> AtmelCtrlFactory::m_emobCreateFunction =
        [](QString devnode, quint8 adr, quint8 ctrlChannelForMux, quint8 debuglevel)
{
    return std::make_shared<AtmelEmobCtrl>(devnode, adr, ctrlChannelForMux, debuglevel);
};

std::shared_ptr<AtmelCommonVersions> AtmelCtrlFactory::createEmobCtrl(QString devnode, quint8 adr, quint8 ctrlChannelForMux, quint8 debuglevel)
{
    return m_emobCreateFunction(devnode, adr, ctrlChannelForMux, debuglevel);
}
