#ifndef PCBSERVER_H
#define PCBSERVER_H

#include "scpiconnection.h"
#include "resource.h"
#include "ethsettings.h"
#include "notificationstructwithvalue.h"
#include <scpi.h>
#include <xiqnetwrapper.h>
#include <netmessages.pb.h>
#include <xmlconfigreader.h>
#include <xiqnetserver.h>
#include <QList>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>

struct ServerParams
{
    QString name;
    QString version;
    QString xsdFile;
};

class cPCBServer: public ScpiConnection
{
    Q_OBJECT
public:
    explicit cPCBServer(ServerParams params, cSCPI *scpiInterface);
    void initSCPIConnection(QString leadingNodes) override;
    cSCPI* getSCPIInterface();
    QString& getName();
    QString& getVersion();

    EthSettings m_ethSettings;
public slots:
    void sendAnswerProto(cProtonetCommand* protoCmd);
protected slots:
    virtual void doConfiguration() = 0; // all servers must configure
    virtual void setupServer(); // all servers must setup
    virtual void setSCPIConnection();
    virtual void SCPIInput();
    virtual void SCPIdisconnect();
    virtual void onSendNotification(ScpiNotificationSubscriber subscriber);
protected:
    void initSCPIConnections();
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
    XiQNetServer* myServer; // the real server that does the communication job
    XiQNetWrapper m_ProtobufWrapper;
    Zera::XMLConfig::cReader m_xmlConfigReader;
    QString m_sConfigurationPath;
    QList<ScpiConnection*> scpiConnectionList; // a list of all scpi connections
    QList<cResource*> resourceList;
    QTcpServer* m_pSCPIServer;
    QTcpSocket* m_pSCPISocket;
private:
    ServerParams m_params;
    QString m_sInput, m_sOutput;
    QTcpSocket* resourceManagerSocket;
    QList<NotificationStructWithValue> m_notifierRegisterNext;
    QList<NotificationStructWithValue> m_notifierRegisterList;

    void registerNotifier(cProtonetCommand* protoCmd); // registeres 1 notifier per command
    void unregisterNotifier(cProtonetCommand *protoCmd); // unregisters all notifiers
    void doUnregisterNotifier(XiQNetPeer *peer, const QByteArray &clientID = QByteArray());
    void sendNotificationToClient(QString message, QByteArray clientID, XiQNetPeer *netPeer);
private slots:
    void onEstablishNewConnection(XiQNetPeer* newClient);
    void onExecuteCommandProto(std::shared_ptr<google::protobuf::Message> cmd);
    void onNotifyPeerConnectionClosed();
    void onEstablishNewNotifier(NotificationValue *notifier);
    void onNotifierChanged(quint32 irqreg);
signals:
    void notifierRegistred(NotificationString* notifier);
    void removeSubscribers(XiQNetPeer* peer, const QByteArray &clientID);
};

#endif // PCBSERVER_H
