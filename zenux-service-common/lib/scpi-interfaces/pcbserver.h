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

enum ServerErrors
{
    noError,
    rmConnectionError,
    atmelError,
    atmelProgError,
};

class cPCBServer : public ScpiConnection
{
    Q_OBJECT
public:
    explicit cPCBServer(SettingsContainerPtr settings, cSCPI *scpiInterface);
    void initSCPIConnection(QString leadingNodes) override;
    cSCPI* getSCPIInterface();
    QString getName();
    QString getVersion();
signals:
    void notifierRegistred(NotificationString* notifier);
    void removeSubscribers(VeinTcp::TcpPeer* peer, const QByteArray &clientID);
public slots:
    void sendAnswerProto(cProtonetCommand* protoCmd);
protected slots:
    virtual void doConfiguration() = 0; // all servers must configure
    virtual void setSCPIConnection();
    virtual void SCPIInput();
    virtual void SCPIdisconnect();
    virtual void onSendNotification(ScpiNotificationSubscriber subscriber);
    virtual void onPeerDisconnected(VeinTcp::TcpPeer *peer);
protected:
    void setupServer();
    void initSCPIConnections();
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;

    SettingsContainerPtr m_settings;
    VeinTcp::TcpServer* m_myServer; // the real server that does the communication job
    Zera::XMLConfig::cReader m_xmlConfigReader;
    QString m_sConfigurationPath;
    QList<ScpiConnection*> scpiConnectionList; // a list of all scpi connections
    QList<cResource*> resourceList;
    QTcpServer* m_pSCPIServer;
    QTcpSocket* m_pSCPISocket;
private slots:
    void onEstablishNewConnection(VeinTcp::TcpPeer *newClient);
    void onMessageReceived(VeinTcp::TcpPeer *peer, QByteArray message);
    void onNotifyPeerConnectionClosed(VeinTcp::TcpPeer *peer);
    void onEstablishNewNotifier(NotificationValue *notifier);
    void onNotifierChanged(quint32 irqreg);
private:
    void registerNotifier(cProtonetCommand* protoCmd); // registeres 1 notifier per command
    void unregisterNotifier(cProtonetCommand *protoCmd); // unregisters all notifiers
    void doUnregisterNotifier(VeinTcp::TcpPeer *peer, const QByteArray &clientID = QByteArray());
    void sendNotificationToClient(QString message, QByteArray clientID, VeinTcp::TcpPeer *netPeer);
    void executeCommandProto(VeinTcp::TcpPeer* peer, std::shared_ptr<google::protobuf::Message> cmd);

    QString m_sInput, m_sOutput;
    QTcpSocket* resourceManagerSocket;
    QList<NotificationStructWithValue> m_notifierRegisterNext;
    QList<NotificationStructWithValue> m_notifierRegisterList;
    XiQNetWrapper m_protobufWrapper;
};

#endif // PCBSERVER_H
