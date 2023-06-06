#ifndef HOTPLUGGABLECONTROLLERCONTAINER_H
#define HOTPLUGGABLECONTROLLERCONTAINER_H

#include "atmelemobctrl.h"
#include "i2csettings.h"
#include "sensesettings.h"
#include <zeramcontrollerbootloaderstopper.h>
#include <QMap>
#include <QVector>
#include <memory>

class HotPluggableControllerContainer : public QObject
{
    Q_OBJECT
public:
    HotPluggableControllerContainer(QString i2cDevNodeName, quint8 i2cAdrCtrl, quint8 i2cAdrMux, quint8 debuglevel);
    void startActualizeEmobControllers(quint16 bitmaskAvailable, const cSenseSettings* senseSettings, int msWaitForApplicationStart);
    QVector<AtmelCommonVersionsPtr> getCurrentControllers();
signals:
    void sigControllersChanged();
private slots:
    void onBootloaderStopAssumed(int ctrlChannel);
private:
    void startAddingController(int ctrlChannel, SenseSystem::cChannelSettings* channelSettings, int msWaitForApplicationStart);

    QString m_i2cDevNodeName;
    quint8 m_i2cAdrCtrl;
    quint8 m_i2cAdrMux;
    quint8 m_debuglevel;
    QMap<int /* ctrlChannel */, AtmelCommonVersionsPtr> m_Controllers;
    struct PendingChannelInfo
    {
        ZeraMControllerBootloaderStopperPtr m_BootloaderStopper;
        qint8 m_nMuxChannelNo;
    };
    QMap<int /* ctrlChannel */, PendingChannelInfo> m_pendingBootloaderStoppers;
};

typedef std::unique_ptr<HotPluggableControllerContainer> HotPluggableControllerContainerPtr;

#endif // HOTPLUGGABLECONTROLLERCONTAINER_H
