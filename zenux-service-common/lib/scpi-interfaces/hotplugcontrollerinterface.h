#ifndef HOTPLUGCONTROLLERINTERFACE_H
#define HOTPLUGCONTROLLERINTERFACE_H

#include "scpiconnection.h"
#include "hotpluggablecontrollercontainer.h"

class HotplugControllerInterface : public ScpiConnection
{
    Q_OBJECT
public:
    HotplugControllerInterface(std::shared_ptr<cSCPI> scpiInterface,
                               HotPluggableControllerContainerPtr hotPluggableControllerContainer);
    void initSCPIConnection(QString leadingNodes) override;

private slots:
    void onControllersChanged();
private:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
    QString emobPushButtonPress(const QString& scpiCmd);
    QString emobReadLockState(const QString& scpiCmd);
    QString emobReadErrorStatus(const QString& scpiCmd);
    QString emobClearErrorStatus(const QString& scpiCmd);
    QString emobReadDataForExchange(const QString& scpiCmd);
    QString findEmobConnected(const QString &channelMNameScpiParam);

    NotificationString m_channels;
    HotPluggableControllerContainerPtr m_hotPluggableControllerContainer;
    QByteArray m_data;
};

#endif // HOTPLUGCONTROLLERINTERFACE_H
