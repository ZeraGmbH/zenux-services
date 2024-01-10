#include "hotpluggablecontrollercontainer.h"
#include "atmelctrlfactorystatic.h"
#include "atmelemobctrl.h"
#include <zeramcontrollerbootloaderstopperfactory.h>
#include <i2cmuxerscopedonoff.h>

HotPluggableControllerContainer::HotPluggableControllerContainer(QString i2cDevNodeName, quint8 i2cAdrCtrl, quint8 i2cAdrMux, quint8 debuglevel) :
    m_i2cDevNodeName(i2cDevNodeName),
    m_i2cAdrCtrl(i2cAdrCtrl),
    m_i2cAdrMux(i2cAdrMux),
    m_debuglevel(debuglevel)
{
}

void HotPluggableControllerContainer::startActualizeEmobControllers(quint16 bitmaskAvailable, const cSenseSettings* senseSettings, int msWaitForApplicationStart)
{
    const auto channelsSettings = senseSettings->getChannelSettings();
    for(const auto channelSettings : channelsSettings) {
        int ctrlChannel = channelSettings->m_nCtrlChannel;
        qint8 plugBitNo = channelSettings->m_nPluggedBit;
        if(plugBitNo < 0)
            continue;
        quint16 bmask = (1 << plugBitNo);
        if (bitmaskAvailable & bmask) {
            if(!isChannelKnown(ctrlChannel))
                startAddingController(ctrlChannel, channelSettings, msWaitForApplicationStart);
        }
        else {
            if(m_ChannelsWithoutController.contains(ctrlChannel)) {
                qInfo("Remove no-controller (clamp) on channel %i", ctrlChannel);
                m_ChannelsWithoutController.remove(ctrlChannel);
            }
            if(m_pendingBootloaderStoppers.contains(ctrlChannel)) {
                qInfo("Remove pending/not yet booted controller on channel %i", ctrlChannel);
                m_pendingBootloaderStoppers.remove(ctrlChannel);
            }
            if(m_Controllers.contains(ctrlChannel)) {
                qInfo("Remove controller on channel %i", ctrlChannel);
                m_Controllers.remove(ctrlChannel);
                emit sigControllersChanged();
            }
        }
    }
}

void HotPluggableControllerContainer::startAddingController(int ctrlChannel, SenseSystem::cChannelSettings* channelSettings, int msWaitForApplicationStart)
{
    I2cMuxerScopedOnOff i2cMuxer(I2cMultiplexerFactory::createPCA9547Muxer(m_i2cDevNodeName, m_i2cAdrMux, channelSettings->m_nMuxChannelNo));
    ZeraMcontrollerIoPtr i2cCtrl = std::make_shared<ZeraMControllerIo>(m_i2cDevNodeName, m_i2cAdrCtrl, m_debuglevel);
    ZeraMControllerBootloaderStopperPtr bootStopper = ZeraMControllerBootloaderStopperFactory::createBootloaderStopper(i2cCtrl, ctrlChannel);
    connect(bootStopper.get(), &ZeraMControllerBootloaderStopper::sigAssumeBootloaderStopped,
            this, &HotPluggableControllerContainer::onBootloaderStopAssumed);
    m_pendingBootloaderStoppers[ctrlChannel] = PendingChannelInfo{ bootStopper, channelSettings->m_nMuxChannelNo };
    bootStopper->stopBootloader(msWaitForApplicationStart);
}

bool HotPluggableControllerContainer::isChannelKnown(int ctrlChannel)
{
    return
            m_pendingBootloaderStoppers.contains(ctrlChannel) ||
            m_Controllers.contains(ctrlChannel) ||
            m_ChannelsWithoutController.contains(ctrlChannel);
}

QVector<AtmelCommonVersionsPtr> HotPluggableControllerContainer::getCurrentControllers()
{
    QVector<AtmelCommonVersionsPtr> controllers;
    for(const auto &ctrl : qAsConst(m_Controllers))
        controllers.append(ctrl);
    return controllers;
}

void HotPluggableControllerContainer::onBootloaderStopAssumed(int ctrlChannel)
{
    qInfo("Bootloader stopped or not available. Try controller version read on channel %i...", ctrlChannel);
    if(m_pendingBootloaderStoppers.contains(ctrlChannel)) {
        AtmelCommonVersionsPtr ctrl = AtmelCtrlFactoryStatic::getEmobCtrl(
                    m_i2cDevNodeName,
                    m_i2cAdrCtrl,
                    m_i2cAdrMux,
                    m_pendingBootloaderStoppers[ctrlChannel].m_nMuxChannelNo,
                    m_debuglevel);
        m_pendingBootloaderStoppers.remove(ctrlChannel);
        QString version;
        ZeraMControllerIo::atmelRM result = ctrl->readCTRLVersion(version);
        if(result == ZeraMControllerIo::cmddone && !version.isEmpty()) {
            qInfo("Version %s read for channel %i - add controller", qPrintable(version), ctrlChannel);
            m_Controllers[ctrlChannel] = ctrl;
            emit sigControllersChanged();
        }
        else {
            qInfo("No version for channel %i - assume no controller (clamp)", ctrlChannel);
            m_ChannelsWithoutController.insert(ctrlChannel);
        }
    }
}

