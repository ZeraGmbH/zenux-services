#ifndef SECGROUPRESOURCEANDINTERFACE_H
#define SECGROUPRESOURCEANDINTERFACE_H

#include "resource.h"
#include "secchannel.h"
#include "seccalcsettings.h"
#include "secinputsettings.h"
#include "protonetcommand.h"
#include <QList>
#include <QHash>
#include <functional>

namespace ECalcSystem
{

const QString Version = "V1.00";
const QString sECalculatorDescription = "Standard error calculator base unit";

enum Commands
{
    cmdVersion,
    cmdChannelCat,
    cmdSetChannels,
    cmdFreeChannels
};

}

class SecGroupResourceAndInterface : public cResource
{
    Q_OBJECT

public:
    SecGroupResourceAndInterface(std::shared_ptr<cSCPI> scpiInterface,
                                 SecCalculatorSettings* ecalcSettings,
                                 SecInputSettings* inputsettings,
                                 std::function<void(int)> funcSigHandler,
                                 AbstractFactoryDeviceNodeSecPtr deviceNodeFactory);
    ~SecGroupResourceAndInterface();
    void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    QList<SecChannel*> getECalcChannelList();
    bool freeChannelsForThisPeer(VeinTcp::TcpPeer *peer);
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    SecCalculatorSettings* m_pecalcsettings;
    SecInputSettings* m_pInputSettings;

    QString m_sVersion;

    QList<SecChannel*> m_ECalculatorChannelList;
    QHash<QString,SecChannel*> m_ECalculatorChannelHash;
    QHash<QByteArray, QString> m_ClientECalcHash; // we hold the set ecalculators by clientid
    QHash<VeinTcp::TcpPeer*, QList<QByteArray>> m_peerClientsHash;

    QString m_ReadVersion(QString& sInput);
    QString m_ReadECalculatorChannelCatalog(QString& sInput);
    void m_SetChannels(cProtonetCommand *protoCmd);
    void m_FreeChannels(cProtonetCommand *protoCmd);
    bool freeChannelsFromAClient(QByteArray clientID);
};

#endif // SECGROUPRESOURCEANDINTERFACE_H
