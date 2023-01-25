#ifndef PCBSERVER_H
#define PCBSERVER_H

#include "scpiconnection.h"
#include "notificationstructwithvalue.h"
#include "resource.h"
#include <scpi.h>
#include <xiqnetwrapper.h>
#include <netmessages.pb.h>
#include <xmlconfigreader.h>
#include <xiqnetserver.h>
#include <QList>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>

class cPCBServer: public ScpiConnection
{
    Q_OBJECT
public:
    explicit cPCBServer(QString name, QString version);
    virtual void initSCPIConnection(QString leadingNodes) override;
    QString& getName();
    QString& getVersion();
public slots:
    void sendAnswerProto(cProtonetCommand* protoCmd);
protected:
    void initSCPIConnections();
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
    XiQNetServer* myServer; // the real server that does the communication job
    XiQNetWrapper m_ProtobufWrapper;
    Zera::XMLConfig::cReader* myXMLConfigReader; // the xml configurator
    QString m_sConfigurationPath;
    QList<ScpiConnection*> scpiConnectionList; // a list of all scpi connections
    QList<cResource*> resourceList;

protected slots:
    virtual void doConfiguration() = 0; // all servers must configure
    virtual void setupServer(); // all servers must setup
private:
    QString m_sServerName;
    QString m_sServerVersion;
    QString m_sInput, m_sOutput;
    QTcpSocket* resourceManagerSocket;

    void registerNotifier(cProtonetCommand* protoCmd); // registeres 1 notifier per command
    void unregisterNotifier(cProtonetCommand *protoCmd); // unregisters all notifiers
    QList<NotificationStructWithValue> m_notifierRegisterNext;
    QList<NotificationStructWithValue> m_notifierRegisterList;

    void doUnregisterNotifier(XiQNetPeer *peer, const QByteArray &clientID = QByteArray());

private slots:
    void onEstablishNewConnection(XiQNetPeer* newClient);
    void onExecuteCommandProto(std::shared_ptr<google::protobuf::Message> cmd);
    void onEstablishNewNotifier(NotificationValue *notifier);
    void onNotifierChanged(quint32 irqreg);
    void onNotifyPeerConnectionClosed();
};

#endif // PCBSERVER_H
