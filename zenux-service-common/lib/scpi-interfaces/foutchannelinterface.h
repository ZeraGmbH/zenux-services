#ifndef FOUTCHANNELINTERFACE_H
#define FOUTCHANNELINTERFACE_H

#include "scpiconnection.h"
#include "notificationstring.h"
#include "foutsettings.h"

class FOutChannelInterface : public ScpiConnection
{
    Q_OBJECT
public:
    FOutChannelInterface(cSCPI *scpiinterface, QString description, quint8 nr, FOutSettings::ChannelSettings* cSettings);
    virtual void initSCPIConnection(QString leadingNodes) override;
    QString& getName();
    QString& getAlias();
    QString& getDescription();
    bool isAvail();
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString readAlias(QString& sInput);
    QString readType(QString& sInput);
    QString readDspServer(QString& sInput);
    QString readDspChannel(QString& sInput);
    QString readChannelStatus(QString& sInput);
    QString readFFactor(QString& sInput);
    QString readWriteConstant(QString& sInput);
    QString readWritePowerType(QString &sInput);
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
