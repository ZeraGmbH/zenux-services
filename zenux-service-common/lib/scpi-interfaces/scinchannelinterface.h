#ifndef SCINCHANNELINTERFACE_H
#define SCINCHANNELINTERFACE_H

#include "scpiconnection.h"
#include "scinsettings.h"

class ScInChannelInterface : public ScpiConnection
{
    Q_OBJECT
public:
    ScInChannelInterface(std::shared_ptr<cSCPI> scpiinterface,
                         QString description,
                         quint8 nr,
                         ScInSettings::ChannelSettings* cSettings);
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


#endif // SCINCHANNELINTERFACE_H
