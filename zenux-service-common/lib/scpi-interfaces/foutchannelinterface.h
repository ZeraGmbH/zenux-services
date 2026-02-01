#ifndef FOUTCHANNELINTERFACE_H
#define FOUTCHANNELINTERFACE_H

#include "scpiconnection.h"
#include "notificationstring.h"
#include "foutsettings.h"

class FOutChannelInterface : public ScpiConnection
{
    Q_OBJECT
public:
    FOutChannelInterface(std::shared_ptr<cSCPI> scpiinterface,
                         QString description,
                         quint8 nr,
                         FOutSettings::ChannelSettings* cSettings);
    virtual void initSCPIConnection(const QString &leadingNodes) override;
    const QString& getName();
    const QString& getAlias();
    const QString& getDescription();
    bool isAvail();
protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
private:
    QString scpiReadAlias(const QString& scpi);
    QString scpiReadType(const QString& scpi);
    QString scpiReadDspServer(const QString& scpi);
    QString scpiReadDspChannel(const QString& scpi);
    QString scpiReadChannelStatus(const QString& scpi);
    QString scpiReadFFactor(const QString& scpi);
    QString scpiReadWriteConstant(const QString& scpi);
    QString scpiReadWritePowerType(const QString &scpi);
    void initNotifier(NotificationString& notifier);
    QString m_sName; // the channel's name
    QString m_sAlias;
    QString m_sDescription; // the channel's brief description
    quint16 m_nDspServer; // the dsp servers port
    quint8 m_nDspChannel; // where to find the channel's sampled data
    quint8 m_nType;
    double m_fFormFactor;
    bool m_bAvail; // is this channel available ?
    NotificationString notifierConstant;
    NotificationString notifierPowerType;
};

#endif // FOUTCHANNELINTERFACE_H
