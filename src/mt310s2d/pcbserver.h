/*****************************************************************************/
/**
 * @file pcbserver.h
 * @brief pcbserver.h holds all necessary declarations for the pcb server class
 * @author Peter Lohmer p.lohmer@zera.de
******************************************************************************/
#ifndef PCBSERVER_H
#define PCBSERVER_H

#include <QObject>
#include <QList>
#include <xiqnetwrapper.h>

#include <scpiconnection.h>
#include <notificationstring.h>
#include "notificationdata.h"

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
class QTcpServer;
class QTcpSocket;
class XiQNetPeer;
class cSCPI;
class cStatusInterface;  // forward
class cDebugSettings;
class cFPGASettings;
class cCtrlSettings;
class cI2CSettings;
class cETHSettings;
class cFRQInputSettings;
class cSCHeadSettings;
class cHKeySettings;
class cSenseSettings;
class cSamplingSettings;
class cSourceSettings;

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


class cPCBServer: public cSCPIConnection
{
    Q_OBJECT

public:
    /**
      @b Initialise the const variables and connections for new clients and their commands
      @param the servers name
      */
    explicit cPCBServer();
    void initSCPIConnection(QString leadingNodes) override;
    cSCPI* getSCPIInterface();
    quint32 getMsgNr();

    /**
      @b reads out the server's name
      */
    QString& getName();
    QString& getVersion();

    cDebugSettings* m_pDebugSettings;
    cFPGASettings* m_pFPGASettings;
    cCtrlSettings* m_pCtrlSettings;
    cI2CSettings* m_pI2CSettings;
    cETHSettings* m_pETHSettings;
    cSenseSettings* m_pSenseSettings;
    cSamplingSettings* m_pSamplingSettings;
    cSourceSettings* m_pSourceSettings;
    cFRQInputSettings* m_pFRQInputSettings;
    cSCHeadSettings* m_pSCHeadSettings;
    cHKeySettings* m_pHKeySettings;
    cStatusInterface* m_pStatusInterface;

signals:
    void sendAnswer(QByteArray answer);

public slots:
    virtual void sendAnswer(cProtonetCommand* protoCmd);

protected:
    void initSCPIConnections();
    XiQNetServer* myServer; // the real server that does the communication job
    XiQNetWrapper m_ProtobufWrapper;
    Zera::XMLConfig::cReader* myXMLConfigReader; // the xml configurator
    QString m_sConfigurationPath;
    QList<cSCPIConnection*> scpiConnectionList; // a list of all scpi connections
    QList<cResource*> resourceList;
    QTcpServer* m_pSCPIServer;
    QTcpSocket* m_pSCPISocket;

protected slots:
    virtual void doConfiguration() = 0; // all servers must configure
    virtual void setupServer(); // all servers must setup
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;
    virtual void setSCPIConnection();
    virtual void SCPIInput();
    virtual void SCPIdisconnect();

private:
    /**
      @b The server's name. The name also specifies the servers service (port number).
      */
    QString m_sServerName;
    QString m_sServerVersion;

    QString m_sInput, m_sOutput;
    QTcpSocket* resourceManagerSocket;

    void m_RegisterNotifier(cProtonetCommand* protoCmd); // registeres 1 notifier per command
    void m_UnregisterNotifier(cProtonetCommand *protoCmd); // unregisters all notifiers
    QList<cNotificationData> notifierRegisterNext;
    QList<cNotificationData> notifierRegisterList;

    void doUnregisterNotifier(XiQNetPeer *peer, const QByteArray &clientID = QByteArray());
    quint32 m_nMsgNr;

private slots:
    virtual void establishNewConnection(XiQNetPeer* newClient);
    virtual void executeCommand(std::shared_ptr<google::protobuf::Message> cmd);
    virtual void establishNewNotifier(cNotificationString* notifier);
    virtual void asyncHandler();
    virtual void peerConnectionClosed();
};

#endif // PCBSERVER_H
