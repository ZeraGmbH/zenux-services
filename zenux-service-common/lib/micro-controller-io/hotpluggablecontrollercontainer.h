#ifndef HOTPLUGGABLECONTROLLERCONTAINER_H
#define HOTPLUGGABLECONTROLLERCONTAINER_H

#include "abstractfactoryi2cctrl.h"
#include "i2csettings.h"
#include "sensesettings.h"
#include <zeramcontrollerbootloaderstopper.h>
#include <QMap>
#include <QSet>
#include <QVector>
#include <memory>

class HotPluggableControllerContainer : public QObject
{
    Q_OBJECT
public:
    HotPluggableControllerContainer(I2cSettings *i2cSettings, AbstractFactoryI2cCtrlPtr ctrlFactory);
    void startActualizeEmobControllers(quint16 bitmaskAvailable, const cSenseSettings* senseSettings, int msWaitForApplicationStart);
    QVector<I2cCtrlCommonInfoPtrShared> getCurrentCommonControllers();
signals:
    void sigControllersChanged();
private slots:
    void onBootloaderStopAssumed(int ctrlChannel);
private:
    void startAddingController(int ctrlChannel, SenseSystem::cChannelSettings* channelSettings, int msWaitForApplicationStart);
    bool isChannelKnown(int ctrlChannel);

    I2cSettings *m_i2cSettings;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    QMap<int /* ctrlChannel */, I2cCtrlCommonInfoPtrShared> m_commonControllers;
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
