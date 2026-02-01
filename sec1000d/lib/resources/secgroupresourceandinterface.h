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
    void initSCPIConnection(const QString &leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    QList<SecChannel*> getECalcChannelList();
    bool freeChannelsForThisPeer(VeinTcp::TcpPeer *peer);
protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;

private:
    SecCalculatorSettings* m_pecalcsettings;
    SecInputSettings* m_pInputSettings;

    QList<SecChannel*> m_ECalculatorChannelList;
    QHash<QByteArray, QString> m_ClientECalcHash; // we hold the set ecalculators by clientid
    QHash<VeinTcp::TcpPeer*, QList<QByteArray>> m_peerClientsHash;

    QString scpiReadVersion(const QString& scpi);
    QString scpiReadECalculatorChannelCatalog(const QString& scpi);
    void scpiSetChannels(ProtonetCommandPtr protoCmd);
    void scpiFreeChannels(ProtonetCommandPtr protoCmd);
    bool freeChannelsFromAClient(QByteArray clientID);
    void connectChannelSignalsAndInitScpi();
};

#endif // SECGROUPRESOURCEANDINTERFACE_H
