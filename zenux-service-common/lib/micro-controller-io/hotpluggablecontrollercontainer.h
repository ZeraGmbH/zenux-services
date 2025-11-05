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

struct HotControllers
{
    I2cCtrlCommonInfoPtrShared m_commonController;
    I2cCtrlEMOBPtr m_emobController;
};
typedef QMap<QString /*channelMName*/, HotControllers> HotControllerMap;

class HotPluggableControllerContainer : public QObject
{
    Q_OBJECT
public:
    HotPluggableControllerContainer(I2cSettings *i2cSettings, AbstractFactoryI2cCtrlPtr ctrlFactory);
    void startActualizeEmobControllers(quint16 bitmaskAvailable, const cSenseSettings* senseSettings, int msWaitForApplicationStart);
    HotControllerMap getCurrentControllers();
signals:
    void sigControllersChanged();
private slots:
    void onBootloaderStopAssumed(int ctrlChannel);
private:
    void startAddingController(int ctrlChannel, SenseSystem::cChannelSettings* channelSettings, int msWaitForApplicationStart);
    bool isChannelKnown(int ctrlChannel);

    I2cSettings *m_i2cSettings;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    struct NamedHotControllers {
        QString channelMName;
        HotControllers controllers;
    };
    QMap<int /* ctrlChannel */, NamedHotControllers> m_controllers;
    struct PendingChannelInfo
    {
        ZeraMControllerBootloaderStopperPtr m_BootloaderStopper;
        QString channelMName;
        qint8 m_nMuxChannelNo;
    };
    QMap<int /* ctrlChannel */, PendingChannelInfo> m_pendingBootloaderStoppers;
    QSet<int /* ctrlChannel */> m_ChannelsWithoutController; /* e.g clamps */
};

typedef std::unique_ptr<HotPluggableControllerContainer> HotPluggableControllerContainerPtr;

#endif // HOTPLUGGABLECONTROLLERCONTAINER_H
