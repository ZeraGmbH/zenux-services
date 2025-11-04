#ifndef HOTPLUGGABLECONTROLLERCONTAINER_H
#define HOTPLUGGABLECONTROLLERCONTAINER_H

#include "abstracthotpluggablecontrollercontainer.h"
#include "abstractfactoryi2cctrl.h"
#include "i2csettings.h"
#include "sensesettings.h"
#include <zeramcontrollerbootloaderstopper.h>
#include <QMap>
#include <QSet>
#include <QVector>
#include <memory>

class HotPluggableControllerContainer : public AbstractHotPluggableControllerContainer
{
    Q_OBJECT
public:
    HotPluggableControllerContainer(I2cSettings *i2cSettings, AbstractFactoryI2cCtrlPtr ctrlFactory);
    void startActualizeEmobControllers(quint16 bitmaskAvailable, const cSenseSettings* senseSettings, int msWaitForApplicationStart) override;
    QVector<I2cCtrlCommonInfoPtrShared> getCurrentCommonControllers() override;
    QVector<I2cCtrlEMOBPtr> getCurrentEmobControllers() override;
private slots:
    void onBootloaderStopAssumed(int ctrlChannel);
private:
    void startAddingController(int ctrlChannel, SenseSystem::cChannelSettings* channelSettings, int msWaitForApplicationStart);
    bool isChannelKnown(int ctrlChannel);

    I2cSettings *m_i2cSettings;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    struct HotControllers
    {
        I2cCtrlCommonInfoPtrShared m_commonController;
        I2cCtrlEMOBPtr m_emobController;
    };
    QMap<int /* ctrlChannel */, HotControllers> m_controllers;
    struct PendingChannelInfo
    {
        ZeraMControllerBootloaderStopperPtr m_BootloaderStopper;
        qint8 m_nMuxChannelNo;
    };
    QMap<int /* ctrlChannel */, PendingChannelInfo> m_pendingBootloaderStoppers;
    QSet<int /* ctrlChannel */> m_ChannelsWithoutController; /* e.g clamps */
};

typedef std::unique_ptr<HotPluggableControllerContainer> HotPluggableControllerContainerPtr;

#endif // HOTPLUGGABLECONTROLLERCONTAINER_H
