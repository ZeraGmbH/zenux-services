/*****************************************************************************/
/**
 * @file pcbserver.h
 * @brief pcbserver.h holds all necessary declarations for the pcb server class
 * @author Peter Lohmer p.lohmer@zera.de
******************************************************************************/
#ifndef PCBSERVER_H
#define PCBSERVER_H

#include "sec1000dglobal.h"
#include "scpiconnection.h"
#include "notificationvalue.h"
#include "notificationdata.h"
#include "notzeronumgen.h"
#include <QObject>
#include <QList>
#include <xiqnetwrapper.h>
#include <scpisingletonfactory.h>

class QTcpSocket;
class QByteArray;
class cResource;

namespace Zera
{
    namespace XMLConfig
    {
        class cReader;
    }
}


namespace google
{
namespace protobuf
{
class Message;
}
}


namespace PCBServer
{
enum commands
{
    cmdRegister,
    cmdUnregister
};
}

class cProtonetCommand;
class XiQNetServer;
class XiQNetPeer;
class cSCPI;
class cStatusInterface;  // forward

/**
  @mainpage base class for pcb servers

  A PCB Server generally has a name and operates on 1 piece of Printed Circuit Board.
  It provides a scpi interface for the resources on the pcb in a generic manner.
  If more than 1 pcb is included in a device, the server has to be run for each device.
  A configuration via xml file is necessary to configure alternate resources, a server
  can export it's resource configuration to a xml file as a template.

  It's interface always has a status model to query it's name, the availability of pcb
  and the status of adjustment of the pcb and it provides a system model to query version
  information and to provide update facilities.

  It's resource model allows for querying all available resources.

  */

class cPCBServer: public ScpiConnection
{
    Q_OBJECT
public:
    explicit cPCBServer();
    virtual void initSCPIConnection(QString leadingNodes) override;
    quint32 getMsgNr();
    QString& getName();
    QString& getVersion();
    cStatusInterface* m_pStatusInterface;
signals:
    void sendAnswer(QByteArray answer);
protected:
    void initSCPIConnections();
    XiQNetServer* myServer; // the real server that does the communication job
    XiQNetWrapper m_ProtobufWrapper;
    Zera::XMLConfig::cReader* myXMLConfigReader; // the xml configurator
    QString m_sConfigurationPath;
    QList<ScpiConnection*> scpiConnectionList; // a list of all scpi connections
    QList<cResource*> resourceList;

protected slots:
    virtual void doConfiguration() = 0; // all servers must configure
    virtual void setupServer(); // all servers must setup
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;
    void sendAnswerProto(cProtonetCommand* protoCmd);
private:
    QString m_sServerName;
    QString m_sServerVersion;
    QString m_sInput, m_sOutput;
    QTcpSocket* resourceManagerSocket;
    NotZeroNumGen m_msgNumGen;

    void registerNotifier(cProtonetCommand* protoCmd); // registeres 1 notifier per command
    void unregisterNotifier(cProtonetCommand *protoCmd); // unregisters all notifiers
    QList<cNotificationData> m_notifierRegisterNext;
    QList<cNotificationData> m_notifierRegisterList;

    void doUnregisterNotifier(XiQNetPeer *peer, const QByteArray &clientID = QByteArray());

private slots:
    virtual void establishNewConnection(XiQNetPeer* newClient);
    void executeCommandProto(std::shared_ptr<google::protobuf::Message> cmd);
    virtual void establishNewNotifier(NotificationValue *notifier);
    virtual void asyncHandler(quint32 irqreg);
    virtual void peerConnectionClosed();
};

#endif // PCBSERVER_H
