#include "hotpluggablecontrollercontainer.h"
#include <zeramcontrollerbootloaderstopperfactory.h>
#include "i2cmultiplexerfactory.h"
#include <i2cmuxerscopedonoff.h>

HotPluggableControllerContainer::HotPluggableControllerContainer(I2cSettings *i2cSettings, AbstractFactoryI2cCtrlPtr ctrlFactory) :
    m_i2cSettings(i2cSettings),
    m_ctrlFactory(ctrlFactory)
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
            if(m_controllers.contains(ctrlChannel)) {
                qInfo("Remove controller on channel %i", ctrlChannel);
                m_controllers.remove(ctrlChannel);
                emit sigControllersChanged();
            }
        }
    }
}

HotControllerMap HotPluggableControllerContainer::getCurrentControllers()
{
    HotControllerMap ctrlMap;
    for (const auto &controller : qAsConst(m_controllers))
        ctrlMap[controller.channelMName] = controller.controllers;
    return ctrlMap;
}

void HotPluggableControllerContainer::startAddingController(int ctrlChannel, SenseSystem::cChannelSettings* channelSettings, int msWaitForApplicationStart)
{
    I2cMuxerScopedOnOff i2cMuxer(I2cMultiplexerFactory::createPCA9547Muxer(m_i2cSettings->getDeviceNode(),
                                                                           m_i2cSettings->getI2CAdress(i2cSettings::muxerI2cAddress),
                                                                           channelSettings->m_nMuxChannelNo));
    ZeraMcontrollerIoPtr i2cCtrl = std::make_shared<ZeraMControllerIo>(m_i2cSettings->getDeviceNode(),
                                                                       m_i2cSettings->getI2CAdress(i2cSettings::emobCtrlI2cAddress),
                                                                       0); // i2c error can occure if clamp is connected
    ZeraMControllerBootloaderStopperPtr bootStopper = ZeraMControllerBootloaderStopperFactory::createBootloaderStopper(i2cCtrl, ctrlChannel);
    connect(bootStopper.get(), &ZeraMControllerBootloaderStopper::sigAssumeBootloaderStopped,
            this, &HotPluggableControllerContainer::onBootloaderStopAssumed);
    m_pendingBootloaderStoppers[ctrlChannel] =
        PendingChannelInfo{ bootStopper, channelSettings->m_nameMx, channelSettings->m_nMuxChannelNo };
    bootStopper->stopBootloader(msWaitForApplicationStart);
}

bool HotPluggableControllerContainer::isChannelKnown(int ctrlChannel)
{
    return
            m_pendingBootloaderStoppers.contains(ctrlChannel) ||
            m_controllers.contains(ctrlChannel) ||
            m_ChannelsWithoutController.contains(ctrlChannel);
}

void HotPluggableControllerContainer::onBootloaderStopAssumed(int ctrlChannel)
{
    qInfo("Bootloader stopped or not available. Try controller version read on channel %i...", ctrlChannel);
    if(m_pendingBootloaderStoppers.contains(ctrlChannel)) {
        qint8 muxChannelNo = m_pendingBootloaderStoppers[ctrlChannel].m_nMuxChannelNo;
        I2cCtrlCommonInfoPtrShared commonCtrl = m_ctrlFactory->getCommonInfoController(
            AbstractFactoryI2cCtrl::CTRL_TYPE_EMOB,
            muxChannelNo);
        PendingChannelInfo channelInfo = m_pendingBootloaderStoppers.take(ctrlChannel);
        QString version;
        ZeraMControllerIo::atmelRM result = commonCtrl->readCTRLVersion(version);
        if(result == ZeraMControllerIo::cmddone && !version.isEmpty()) {
            qInfo("Version %s read for controller channel %i / mux channel %i - add controller",
                  qPrintable(version), ctrlChannel, muxChannelNo);
            I2cCtrlEMOBPtr emobCtrl = m_ctrlFactory->getEmobController(muxChannelNo);
            HotControllers controllers{commonCtrl, emobCtrl};

            m_controllers[ctrlChannel] = {channelInfo.channelMName, controllers};
            emit sigControllersChanged();
        }
        else {
            qInfo("No version for channel %i - assume no controller (clamp)", ctrlChannel);
            m_ChannelsWithoutController.insert(ctrlChannel);
        }
    }
}

