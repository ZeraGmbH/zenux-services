#include "atmelctrlfactory.h"

std::function<AtmelCommonVersionsPtr(QString, quint8, quint8, quint8, quint8)> AtmelCtrlFactory::m_emobCreateFunction =
        [](QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 ctrlChannel, quint8 debuglevel)
{
    return std::make_shared<AtmelEmobCtrl>(devnode, adrCtrl, adrMux, ctrlChannel, debuglevel);
};

AtmelCommonVersionsPtr AtmelCtrlFactory::createEmobCtrl(QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 muxChannel, quint8 debuglevel)
{
    return m_emobCreateFunction(devnode, adrCtrl, adrMux, muxChannel, debuglevel);
}
