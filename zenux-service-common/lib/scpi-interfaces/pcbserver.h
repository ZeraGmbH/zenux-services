#ifndef PCBSERVER_H
#define PCBSERVER_H

#include "scpiconnection.h"
#include "notificationstructwithstringandid.h"
#include "resource.h"
#include "ethsettings.h"
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
    explicit cPCBServer(QString name, QString version, cSCPI *scpiInterface);
    void initSCPIConnection(QString leadingNodes) override;
    cSCPI* getSCPIInterface();
    QString& getName();
    QString& getVersion();

    EthSettings* m_pETHSettings;
    virtual void sendNotificationToClient(NotificationStructWithStringAndId notData);

public slots:
    void sendAnswerProto(cProtonetCommand* protoCmd);
protected slots:
    virtual void doConfiguration() = 0; // all servers must configure
    virtual void setupServer(); // all servers must setup
    virtual void setSCPIConnection();
    virtual void SCPIInput();
    virtual void SCPIdisconnect();
protected:
    void initSCPIConnections();
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
    XiQNetServer* myServer; // the real server that does the communication job
    XiQNetWrapper m_ProtobufWrapper;
    Zera::XMLConfig::cReader* myXMLConfigReader; // the xml configurator
    QString m_sConfigurationPath;
    QList<ScpiConnection*> scpiConnectionList; // a list of all scpi connections
    QList<cResource*> resourceList;
    QTcpServer* m_pSCPIServer;
    QTcpSocket* m_pSCPISocket;
private:
    QString m_sServerName;
    QString m_sServerVersion;
    QString m_sInput, m_sOutput;
    QTcpSocket* resourceManagerSocket;

    void registerNotifier(cProtonetCommand* protoCmd); // registeres 1 notifier per command
    void unregisterNotifier(cProtonetCommand *protoCmd); // unregisters all notifiers
    void doUnregisterNotifier(XiQNetPeer *peer, const QByteArray &clientID = QByteArray());
    QList<NotificationStructWithStringAndId> m_notifierRegisterNext;
    QList<NotificationStructWithStringAndId> m_notifierRegisterList;
private slots:
    void onEstablishNewConnection(XiQNetPeer* newClient);
    void onExecuteCommandProto(std::shared_ptr<google::protobuf::Message> cmd);
    void onEstablishNewNotifier(NotificationString* notifier);
    void onNotifierChanged();
    void onNotifyPeerConnectionClosed();
signals:
    void notifierRegistred(NotificationString* notifier);
    void notifierUnregistred(NotificationString* notifier);
};

#endif // PCBSERVER_H
