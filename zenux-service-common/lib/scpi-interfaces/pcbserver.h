#ifndef PCBSERVER_H
#define PCBSERVER_H

#include "scpiconnection.h"
#include "resource.h"
#include "notificationstructwithvalue.h"
#include "settingscontainer.h"
#include <scpi.h>
#include <xiqnetwrapper.h>
#include <netmessages.pb.h>
#include <xmlconfigreader.h>
#include <vtcp_server.h>
#include <QList>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>

class PCBServer : public ScpiConnection
{
    Q_OBJECT
public:
    explicit PCBServer(SettingsContainerPtr settings,
                       VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory);
    void initSCPIConnection(QString leadingNodes) override;
    std::shared_ptr<cSCPI> getSCPIInterface();
    QString getName();
    QString getVersion();
signals:
    void notifierRegistred(NotificationString* notifier);
    void removeSubscribers(VeinTcp::TcpPeer* peer, const QByteArray &clientID);
public slots:
    void sendProtoAnswer(cProtonetCommand* protoCmd);
protected slots:
    virtual void onNotifySubscriber(ScpiNotificationSubscriber subscriber, QString newValue);
    virtual void onProtobufDisconnect(VeinTcp::TcpPeer *peer);
protected:
    void setupServer();
    void initSCPIConnections();
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
    void openTelnetScpi();

    SettingsContainerPtr m_settings;
    Zera::XMLConfig::cReader m_xmlConfigReader;
    QList<ScpiConnection*> m_scpiConnectionList; // a list of all scpi connections
    QList<cResource*> m_resourceList;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    VeinTcp::TcpServer m_protoBufServer;
private slots:
    void onProtobufClientConnected(VeinTcp::TcpPeer *newClient);
    void onProtobufDataReceived(VeinTcp::TcpPeer *peer, QByteArray message);
    void onNotifyPeerConnectionClosed(VeinTcp::TcpPeer *peer);
    void onEstablishNewNotifier(NotificationValue *notifier);
    void onNotifierChanged(quint32 irqreg);
    void onTelnetClientConnected();
    void onTelnetDataReceived();
    void onTelnetDisconnect();
private:
    void registerNotifier(cProtonetCommand* protoCmd); // registeres 1 notifier per command
    void unregisterNotifier(cProtonetCommand *protoCmd); // unregisters all notifiers
    void doUnregisterNotifier(VeinTcp::TcpPeer *peer, const QByteArray &clientID = QByteArray());
    void sendNotificationToClient(QString message, QByteArray clientID, VeinTcp::TcpPeer *netPeer);
    void executeCommandProto(VeinTcp::TcpPeer* peer, std::shared_ptr<google::protobuf::Message> cmd);

    QTcpServer* m_telnetServer = nullptr;
    QTcpSocket* m_telnetSocket = nullptr;
    QList<NotificationStructWithValue> m_notifierRegisterNext;
    QList<NotificationStructWithValue> m_notifierRegisterList;
    XiQNetWrapper m_protobufWrapper;
};

#endif // PCBSERVER_H
