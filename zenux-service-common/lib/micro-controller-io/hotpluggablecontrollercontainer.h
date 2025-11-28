#ifndef HOTPLUGGABLECONTROLLERCONTAINER_H
#define HOTPLUGGABLECONTROLLERCONTAINER_H

#include "abstractfactoryi2cctrl.h"
#include "emobdefinitions.h"
#include "i2csettings.h"
#include "sensesettings.h"
#include <i2cctlbootloaderstopper.h>
#include <QMap>
#include <QSet>
#include <QVector>
#include <memory>

struct HotControllers
{
    I2cCtrlCommonInfoPtrShared m_commonController;
    I2cCtrlEMOBPtr m_emobController;
    EmobControllerTypes m_controllerType;
};
typedef QMap<QString /*channelMName*/, HotControllers> HotControllerMap;

class HotPluggableControllerContainer : public QObject
{
    Q_OBJECT
public:
    HotPluggableControllerContainer(I2cSettingsPtr i2cSettings, AbstractFactoryI2cCtrlPtr ctrlFactory);
    void startActualizeEmobControllers(quint16 bitmaskAvailable,
                                       cSenseSettingsPtr senseSettings,
                                       int msWaitForApplicationStart);
    HotControllerMap getCurrentControllers();
    static EmobControllerTypes getEmobControllerType(const QString &subInstrumentReceived);
signals:
    void sigControllersChanged();

private slots:
    void onBootloaderStopAssumed(int ctrlChannel);
private:
    void startAddingController(int ctrlChannel, SenseSystem::cChannelSettings* channelSettings, int msWaitForApplicationStart);
    bool isChannelKnown(int ctrlChannel);

    const I2cSettingsPtr m_i2cSettings;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    struct NamedHotControllers {
        QString channelMName;
        HotControllers controllers;
    };
    QMap<int /* ctrlChannel */, NamedHotControllers> m_controllers;
    struct PendingChannelInfo
    {
        I2cCtlBootloaderStopperPtr m_BootloaderStopper;
        QString channelMName;
        qint8 m_nMuxChannelNo;
    };
    QMap<int /* ctrlChannel */, PendingChannelInfo> m_pendingBootloaderStoppers;
    QSet<int /* ctrlChannel */> m_ChannelsWithoutController; /* e.g clamps */
};

typedef std::shared_ptr<HotPluggableControllerContainer> HotPluggableControllerContainerPtr;

#endif // HOTPLUGGABLECONTROLLERCONTAINER_H
