#ifndef PCBSERVER_H
#define PCBSERVER_H

#include "scpiconnection.h"
#include "notificationstring.h"
#include "notificationdata.h"
#include "resource.h"
#include "debugsettings.h"
#include "fpgasettings.h"
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
class cI2CSettings;
class cFRQInputSettings;
class cSCHeadSettings;
class cHKeySettings;
class cSenseSettings;
class cSamplingSettings;
class cSourceSettings;

class cPCBServer: public ScpiConnection
{
    Q_OBJECT
public:
    explicit cPCBServer();
    virtual void initSCPIConnection(QString leadingNodes) override;
    cSCPI* getSCPIInterface();
    quint32 getMsgNr();
    QString& getName();
    QString& getVersion();

    cDebugSettings* m_pDebugSettings;
    cFPGASettings* m_pFPGASettings;
    cI2CSettings* m_pI2CSettings;
    EthSettings* m_pETHSettings;
    cSenseSettings* m_pSenseSettings;
    cSamplingSettings* m_pSamplingSettings;
    cSourceSettings* m_pSourceSettings;
    cFRQInputSettings* m_pFRQInputSettings;
    cSCHeadSettings* m_pSCHeadSettings;
    cHKeySettings* m_pHKeySettings;
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
    QTcpServer* m_pSCPIServer;
    QTcpSocket* m_pSCPISocket;

protected slots:
    virtual void doConfiguration() = 0; // all servers must configure
    virtual void setupServer(); // all servers must setup
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;
    void sendAnswerProto(cProtonetCommand* protoCmd);
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
    QList<NotificationStructForPcb> m_notifierRegisterNext;
    QList<NotificationStructForPcb> m_notifierRegisterList;

    void doUnregisterNotifier(XiQNetPeer *peer, const QByteArray &clientID = QByteArray());

private slots:
    virtual void establishNewConnection(XiQNetPeer* newClient);
    void executeCommandProto(std::shared_ptr<google::protobuf::Message> cmd);
    virtual void establishNewNotifier(NotificationString* notifier);
    virtual void asyncHandler();
    virtual void notifyPeerConnectionClosed();
};

#endif // PCBSERVER_H
