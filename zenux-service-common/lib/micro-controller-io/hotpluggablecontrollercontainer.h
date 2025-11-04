#ifndef HOTPLUGGABLECONTROLLERCONTAINER_H
#define HOTPLUGGABLECONTROLLERCONTAINER_H

#include "abstracthotpluggablecontrollercontainer.h"
#include "abstractfactoryi2cctrl.h"
#include "i2csettings.h"
#include "sensesettings.h"
#include <zeramcontrollerbootloaderstopper.h>
#include <QMap>
#include <QSet>

class HotPluggableControllerContainer : public AbstractHotPluggableControllerContainer
{
    Q_OBJECT
public:
    HotPluggableControllerContainer(I2cSettings *i2cSettings, AbstractFactoryI2cCtrlPtr ctrlFactory);
    void startActualizeEmobControllers(quint16 bitmaskAvailable,
                                       const cSenseSettings* senseSettings,
                                       int msWaitForApplicationStart) override;
    HotControllerMap getCurrentControllers() override;
private slots:
    void onBootloaderStopAssumed(int ctrlChannel);
private:
    void startAddingController(int ctrlChannel, SenseSystem::cChannelSettings* channelSettings, int msWaitForApplicationStart);
    bool isChannelKnown(int ctrlChannel);

    I2cSettings *m_i2cSettings;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    HotControllerMap m_controllers;
    struct PendingChannelInfo
    {
        ZeraMControllerBootloaderStopperPtr m_BootloaderStopper;
        qint8 m_nMuxChannelNo;
    };
    QMap<int /* ctrlChannel */, PendingChannelInfo> m_pendingBootloaderStoppers;
    QSet<int /* ctrlChannel */> m_ChannelsWithoutController; /* e.g clamps */
};

#endif // HOTPLUGGABLECONTROLLERCONTAINER_H
