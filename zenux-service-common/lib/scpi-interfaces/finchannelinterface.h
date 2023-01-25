#ifndef FINCHANNELINTERFACE_H
#define FINCHANNELINTERFACE_H

#include "scpiconnection.h"
#include "finsettings.h"

class FInChannelInterface : public ScpiConnection
{
    Q_OBJECT
public:
    FInChannelInterface(cSCPI* scpiInterface, QString description, quint8 nr, FInSettings::ChannelSettings* cSettings);
    virtual void initSCPIConnection(QString leadingNodes) override;
    QString& getName();
    QString& getAlias();
    QString& getDescription();
    bool isAvail();
protected:
    void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString readAlias(QString& sInput);
    QString readChannelStatus(QString& sInput);
    QString m_sName; // the channel's name
    QString m_sAlias;
    QString m_sDescription; // the channel's brief description
    bool m_bAvail; // is this channel available ?
};

#endif // FINCHANNELINTERFACE_H
