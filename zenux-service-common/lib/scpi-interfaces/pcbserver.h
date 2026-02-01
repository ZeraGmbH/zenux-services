#ifndef PCBSERVER_H
#define PCBSERVER_H

#include "scpiconnection.h"
#include "resource.h"
#include "notificationstructwithvalue.h"
#include "settingscontainer.h"
#include "consoleserver.h"
#include <scpi.h>
#include <xiqnetwrapper.h>
#include <netmessages.pb.h>
#include <xmlconfigreader.h>
#include <vtcp_server.h>
#include <QList>
#include <QByteArray>

class PCBServer : public ScpiConnection
{
    Q_OBJECT
public:
    explicit PCBServer(SettingsContainerPtr settings,
                       VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory);
    void initSCPIConnection(const QString &leadingNodes) override;
    std::shared_ptr<cSCPI> getSCPIInterface();
    QString getName();
    QString getVersion();
    I2cSettingsPtr getI2cSettings() const;
    cSenseSettingsPtr getSenseSettings() const;
signals:
    void notifierRegistred(NotificationString* notifier);
    void removeSubscribers(VeinTcp::TcpPeer* peer, const QByteArray &clientID);
public slots:
    void sendProtoAnswer(ProtonetCommandPtr protoCmd);
protected slots:
    virtual void onNotifySubscriber(const ScpiNotificationSubscriber &subscriber, const QString &newValue);
    virtual void onProtobufDisconnect(VeinTcp::TcpPeer *peer);
protected:
    void connectProtoConnectionSignals();
    void initSCPIConnections();
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
    void openTelnetScpi();

    SettingsContainerPtr m_settings;
    Zera::XMLConfig::cReader m_xmlConfigReader;
    QList<ScpiConnection*> m_scpiConnectionList; // a list of all scpi connections
    QList<cResource*> m_resourceList;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    VeinTcp::TcpServer m_protoBufServer;
    ConsoleServer m_telnetServer;
private slots:
    void onProtobufClientConnected(VeinTcp::TcpPeer *newClient);
    void onProtobufDataReceived(VeinTcp::TcpPeer *peer, const QByteArray &message);
    void onNotifyPeerConnectionClosed(VeinTcp::TcpPeer *peer);
    void onEstablishNewNotifier(NotificationValue *notifier);
    void onNotifierChanged(quint32 irqreg);
    void onTelnetReceived(const QString &input);
private:
    void registerNotifier(ProtonetCommandPtr protoCmd); // registeres 1 notifier per command
    void unregisterNotifier(ProtonetCommandPtr protoCmd); // unregisters all notifiers
    void doUnregisterNotifier(VeinTcp::TcpPeer *peer, const QByteArray &clientID = QByteArray());
    void sendNotificationToClient(const QString &message, const QByteArray &clientID, VeinTcp::TcpPeer *netPeer);
    void executeCommandProto(VeinTcp::TcpPeer* peer, std::shared_ptr<google::protobuf::Message> cmd);

    QList<NotificationStructWithValue> m_notifierRegisterNext;
    QList<NotificationStructWithValue> m_notifierRegisterList;
    XiQNetWrapper m_protobufWrapper;
};

#endif // PCBSERVER_H
