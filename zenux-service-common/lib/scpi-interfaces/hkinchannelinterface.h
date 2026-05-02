#ifndef HKINCHANNELINTERFACE_H
#define HKINCHANNELINTERFACE_H

#include "scpiconnection.h"
#include "hkinsettings.h"

class HkInChannelInterface : public ScpiConnection
{
    Q_OBJECT
public:
    HkInChannelInterface(const std::shared_ptr<cSCPI> &scpiInterface,
                         QString description,
                         quint8 nr,
                         HkInSettings::ChannelSettings* cSettings);
    void initSCPIConnection(const QString &leadingNodes);
    QString& getName();
    QString& getAlias();
    QString& getDescription();
    bool isAvail();
protected:
    void executeProtoScpi(int cmdCode, const ProtonetCommandPtr &protoCmd) override;
private:
    QString scpiReadAlias(const QString& scpi);
    QString scpiReadChannelStatus(const QString& scpi);
    QString m_sName; // the channel's name
    QString m_sAlias;
    QString m_sDescription; // the channel's brief description
    bool m_bAvail; // is this channel available ?
};

#endif // HKINCHANNELINTERFACE_H
