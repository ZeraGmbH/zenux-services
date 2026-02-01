#ifndef HKINCHANNELINTERFACE_H
#define HKINCHANNELINTERFACE_H

#include "scpiconnection.h"
#include "hkinsettings.h"

class HkInChannelInterface : public ScpiConnection
{
    Q_OBJECT
public:
    HkInChannelInterface(std::shared_ptr<cSCPI> scpiinterface,
                         QString description,
                         quint8 nr,
                         HkInSettings::ChannelSettings* cSettings);
    virtual void initSCPIConnection(const QString &leadingNodes) override;
    QString& getName();
    QString& getAlias();
    QString& getDescription();
    bool isAvail();
protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
private:
    QString scpiReadAlias(const QString& scpi);
    QString scpiReadChannelStatus(const QString& scpi);
    QString m_sName; // the channel's name
    QString m_sAlias;
    QString m_sDescription; // the channel's brief description
    bool m_bAvail; // is this channel available ?
};

#endif // HKINCHANNELINTERFACE_H
