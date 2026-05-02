#ifndef SCINCHANNELINTERFACE_H
#define SCINCHANNELINTERFACE_H

#include "scpiconnection.h"
#include "scinsettings.h"

class ScInChannelInterface : public ScpiConnection
{
    Q_OBJECT
public:
    ScInChannelInterface(const std::shared_ptr<cSCPI> &scpiInterface,
                         QString description,
                         quint8 nr,
                         ScInSettings::ChannelSettings* cSettings);
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


#endif // SCINCHANNELINTERFACE_H
