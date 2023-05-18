#include "hotpluggablecontrollercontainer.h"
#include "atmelctrlfactory.h"
#include "atmelemobctrl.h"

HotPluggableControllerContainer::HotPluggableControllerContainer(QString i2cDevNodeName, quint8 i2cAdrCtrl, quint8 i2cAdrMux, quint8 debuglevel) :
    m_i2cDevNodeName(i2cDevNodeName),
    m_i2cAdrCtrl(i2cAdrCtrl),
    m_i2cAdrMux(i2cAdrMux),
    m_debuglevel(debuglevel)
{
}

void HotPluggableControllerContainer::actualizeEmobControllers(const cSenseSettings* senseSettings, quint16 bitmaskAvailable)
{
    const auto channelsSettings = senseSettings->getChannelSettings();
    for(const auto channelSettings : channelsSettings) {
        int ctrlChannel = channelSettings->m_nCtrlChannel;

        qint8 plugBitNo = channelSettings->m_nPluggedBit;
        if(plugBitNo < 0)
            continue;

        quint16 bmask = (1 << plugBitNo);
        if (bitmaskAvailable & bmask) {
            m_Controllers[ctrlChannel] = AtmelCtrlFactory::createEmobCtrl(
                        m_i2cDevNodeName,
                        m_i2cAdrCtrl,
                        m_i2cAdrMux,
                        channelSettings->m_nMuxChannelNo,
                        m_debuglevel);
        }
        else
            m_Controllers.remove(ctrlChannel);
    }
}

QVector<AtmelCommonVersionsPtr > HotPluggableControllerContainer::getCurrentControllers()
{
    QVector<AtmelCommonVersionsPtr> controllers;
    for(const auto &ctrl : qAsConst(m_Controllers))
        controllers.append(ctrl);
    return controllers;
}
