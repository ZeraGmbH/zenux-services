#include "hotpluggablecontrollercontainer.h"
#include "atmelctrlfactory.h"
#include "atmelemobctrl.h"
#include <i2cmuxerscopedonoff.h>

HotPluggableControllerContainer::HotPluggableControllerContainer(QString i2cDevNodeName, quint8 i2cAdrCtrl, quint8 i2cAdrMux, quint8 debuglevel) :
    m_i2cDevNodeName(i2cDevNodeName),
    m_i2cAdrCtrl(i2cAdrCtrl),
    m_i2cAdrMux(i2cAdrMux),
    m_debuglevel(debuglevel)
{
}

void HotPluggableControllerContainer::startActualizeEmobControllers(const cSenseSettings* senseSettings, quint16 bitmaskAvailable)
{
    const auto channelsSettings = senseSettings->getChannelSettings();
    for(const auto channelSettings : channelsSettings) {
        int ctrlChannel = channelSettings->m_nCtrlChannel;
        qint8 plugBitNo = channelSettings->m_nPluggedBit;
        if(plugBitNo < 0)
            continue;
        quint16 bmask = (1 << plugBitNo);
        if (bitmaskAvailable & bmask)
            tryStartAddingController(ctrlChannel, channelSettings);
        else {
            m_Controllers.remove(ctrlChannel);
            emit sigControllersChanged();
        }
    }
}

void HotPluggableControllerContainer::tryStartAddingController(int ctrlChannel, SenseSystem::cChannelSettings* channelSettings)
{
    if(m_Controllers.contains(ctrlChannel))
        return;
    I2cMuxerScopedOnOff i2cMuxer(I2cMultiplexerFactory::createPCA9547Muxer(m_i2cDevNodeName, m_i2cAdrMux, channelSettings->m_nMuxChannelNo));
    ZeraMcontrollerIoPtr i2cCtrl = std::make_shared<ZeraMControllerIo>(m_i2cDevNodeName, m_i2cAdrCtrl, m_debuglevel);
    std::shared_ptr<ZeraMControllerBootloaderStopper> bootStopper = std::make_shared<ZeraMControllerBootloaderStopper>(i2cCtrl, ctrlChannel);
    connect(bootStopper.get(), &ZeraMControllerBootloaderStopper::sigAssumeBootloaderStopped,
            this, &HotPluggableControllerContainer::onBootloaderStoppAssumed);
    m_pendingBootloaderStoppers[ctrlChannel] = PendingChannelInfo{ bootStopper, channelSettings->m_nMuxChannelNo };
    bootStopper->stopBootloader(10000);
}

QVector<AtmelCommonVersionsPtr> HotPluggableControllerContainer::getCurrentControllers()
{
    QVector<AtmelCommonVersionsPtr> controllers;
    for(const auto &ctrl : qAsConst(m_Controllers))
        controllers.append(ctrl);
    return controllers;
}

void HotPluggableControllerContainer::onBootloaderStoppAssumed(int ctrlChannel)
{
    if(m_pendingBootloaderStoppers.contains(ctrlChannel)) {
        AtmelCommonVersionsPtr ctrl = AtmelCtrlFactory::createEmobCtrl(
                    m_i2cDevNodeName,
                    m_i2cAdrCtrl,
                    m_i2cAdrMux,
                    m_pendingBootloaderStoppers[ctrlChannel].m_nMuxChannelNo,
                    m_debuglevel);
        m_pendingBootloaderStoppers.remove(ctrlChannel);
        QString version;
        ZeraMControllerIo::atmelRM result = ctrl->readCTRLVersion(version);
        if(result == ZeraMControllerIo::cmddone && !version.isEmpty()) {
            m_Controllers[ctrlChannel] = ctrl;
            emit sigControllersChanged();
        }
    }
}

