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
    void startActualizeEmobControllers(const cSenseSettings* senseSettings, quint16 bitmaskAvailable);
    QVector<AtmelCommonVersionsPtr> getCurrentControllers();
signals:
    void sigControllersChanged();
private slots:
    void onBootloaderStoppAssumed(int ctrlChannel);
private:
    void tryStartAddingController(int ctrlChannel, SenseSystem::cChannelSettings* channelSettings);

    QString m_i2cDevNodeName;
    quint8 m_i2cAdrCtrl;
    quint8 m_i2cAdrMux;
    quint8 m_debuglevel;
    QMap<int /* ctrlChannel */, AtmelCommonVersionsPtr> m_Controllers;
    struct PendingChannelInfo
    {
        std::shared_ptr<ZeraMControllerBootloaderStopper> m_BootloaderStopper;
        qint8 m_nMuxChannelNo;
    };
    QMap<int /* ctrlChannel */, PendingChannelInfo> m_pendingBootloaderStoppers;
};

#endif // HOTPLUGGABLECONTROLLERCONTAINER_H
