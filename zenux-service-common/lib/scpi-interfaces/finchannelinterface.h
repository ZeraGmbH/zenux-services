#ifndef FINCHANNELINTERFACE_H
#define FINCHANNELINTERFACE_H

#include "scpiconnection.h"
#include "finsettings.h"

class FInChannelInterface : public ScpiConnection
{
    Q_OBJECT
public:
    FInChannelInterface(std::shared_ptr<cSCPI> scpiInterface,
                        const QString &description,
                        quint8 nr,
                        FInSettings::ChannelSettings* cSettings);
    virtual void initSCPIConnection(QString leadingNodes) override;
    const QString& getName();
    const QString& getAlias();
    const QString& getDescription();
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

#endif // FINCHANNELINTERFACE_H
