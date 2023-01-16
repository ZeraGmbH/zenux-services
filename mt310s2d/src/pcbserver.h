#ifndef PCBSERVER_H
#define PCBSERVER_H

#include "scpiconnection.h"
#include "notificationstring.h"
#include "notificationdata.h"
#include "resource.h"
#include "debugsettings.h"
#include "fpgasettings.h"
#include "ctrlsettings.h"
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

namespace PCBServer
{
enum commands
{
    cmdRegister,
    cmdUnregister
};
}

class cStatusInterface;  // forward
class cCtrlSettings;
class cI2CSettings;
class cFRQInputSettings;
class cSCHeadSettings;
class HkInSettings;
class cSenseSettings;
class cSamplingSettings;
class FOutSettings;

class cPCBServer: public ScpiConnection
{
    Q_OBJECT
public:
    explicit cPCBServer();
    void initSCPIConnection(QString leadingNodes) override;
    cSCPI* getSCPIInterface();
    QString& getName();
    QString& getVersion();

    cDebugSettings* m_pDebugSettings;
    cFPGASettings* m_pFPGASettings;
    cCtrlSettings* m_pCtrlSettings;
    cI2CSettings* m_pI2CSettings;
    EthSettings* m_pETHSettings;
    cSenseSettings* m_pSenseSettings;
    cSamplingSettings* m_pSamplingSettings;
    FOutSettings* m_pSourceSettings;
    cFRQInputSettings* m_pFRQInputSettings;
    cSCHeadSettings* m_pSCHeadSettings;
    HkInSettings* m_HkInSettings;
    cStatusInterface* m_pStatusInterface;
public slots:
    void sendAnswerProto(cProtonetCommand* protoCmd);
protected:
    void initSCPIConnections();
    XiQNetServer* myServer; // the real server that does the communication job
    XiQNetWrapper m_ProtobufWrapper;
    Zera::XMLConfig::cReader* myXMLConfigReader; // the xml configurator
    QString m_sConfigurationPath;
    QList<ScpiConnection*> scpiConnectionList; // a list of all scpi connections
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
    QString m_sServerName;
    QString m_sServerVersion;

    QString m_sInput, m_sOutput;
    QTcpSocket* resourceManagerSocket;

    void registerNotifier(cProtonetCommand* protoCmd); // registeres 1 notifier per command
    void unregisterNotifier(cProtonetCommand *protoCmd); // unregisters all notifiers
    void doUnregisterNotifier(XiQNetPeer *peer, const QByteArray &clientID = QByteArray());
    QList<NotificationStructForPcb> m_notifierRegisterNext;
    QList<NotificationStructForPcb> m_notifierRegisterList;
private slots:
    virtual void establishNewConnection(XiQNetPeer* newClient);
    void executeCommandProto(std::shared_ptr<google::protobuf::Message> cmd);
    virtual void establishNewNotifier(NotificationString* notifier);
    virtual void asyncHandler();
    virtual void notifyPeerConnectionClosed();
};

#endif // PCBSERVER_H
