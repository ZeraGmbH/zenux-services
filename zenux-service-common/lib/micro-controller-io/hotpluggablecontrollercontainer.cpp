#include "hotpluggablecontrollercontainer.h"
#include "emobdefinitions.h"

HotPluggableControllerContainer::HotPluggableControllerContainer(const I2cSettings *i2cSettings, AbstractFactoryI2cCtrlPtr ctrlFactory) :
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

EmobControllerTypes HotPluggableControllerContainer::getEmobControllerType(const QString &subInstrumentReceived)
{
    if(subInstrumentReceived.contains("MT650e"))
        return MT650e;
    return EMOB;
}

void HotPluggableControllerContainer::startAddingController(int ctrlChannel,
                                                            SenseSystem::cChannelSettings* channelSettings,
                                                            int msWaitForApplicationStart)
{
    I2cCtlBootloaderStopperPtr bootStopper = std::make_shared<I2cCtlBootloaderStopper>(m_ctrlFactory,
                                                                                       ctrlChannel,
                                                                                       channelSettings->m_nMuxChannelNo);
    connect(bootStopper.get(), &I2cCtlBootloaderStopper::sigAssumeBootloaderStopped,
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
    qInfo("Bootloader assumed inactive now on channel %i.", ctrlChannel);
    if(m_pendingBootloaderStoppers.contains(ctrlChannel)) {
        qint8 muxChannelNo = m_pendingBootloaderStoppers[ctrlChannel].m_nMuxChannelNo;
        qInfo("Read controller version on channel %i / mux channel %i...", ctrlChannel, muxChannelNo);
        I2cCtrlCommonInfoPtrShared commonCtrl = m_ctrlFactory->getCommonInfoController(
            AbstractFactoryI2cCtrl::CTRL_TYPE_EMOB,
            muxChannelNo);
        PendingChannelInfo channelInfo = m_pendingBootloaderStoppers.take(ctrlChannel);
        QString version;
        ZeraMControllerIo::atmelRM result = commonCtrl->readCTRLVersion(version);
        if(result == ZeraMControllerIo::cmddone && !version.isEmpty()) {
            qInfo("Version %s read for controller channel %i / mux channel %i",
                  qPrintable(version), ctrlChannel, muxChannelNo);
            I2cCtrlEMOBPtr emobCtrl = m_ctrlFactory->getEmobController(muxChannelNo);
            if (emobCtrl) {
                QString instrumentSubType;
                result = emobCtrl->readEmobInstrumentSubType(instrumentSubType);
                if(result == ZeraMControllerIo::cmddone && !instrumentSubType.isEmpty()) {
                    qInfo("Instrument type read: '%s'. Add controller on channel %i / mux channel %i",
                          qPrintable(instrumentSubType), ctrlChannel, muxChannelNo);
                    HotControllers controllers{commonCtrl, emobCtrl, EMOBUnknown};
                    m_controllers[ctrlChannel] = {channelInfo.channelMName, controllers};
                    m_controllers[ctrlChannel].controllers.m_controllerType = getEmobControllerType(instrumentSubType);
                    emit sigControllersChanged();
                }
            }
        }
        else {
            qInfo("No version for channel %i - assume clamp only", ctrlChannel);
            m_ChannelsWithoutController.insert(ctrlChannel);
        }
    }
}
