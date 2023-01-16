#ifndef FINCHANNELINTERFACE_H
#define FINCHANNELINTERFACE_H

#include "scpiconnection.h"
#include "finsettings.h"

namespace FPZINChannel
{
enum Commands
{
    cmdAlias,
    cmdStatus
};

}

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

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    QString m_sName; // the channel's name
    QString m_sAlias;
    QString m_sDescription; // the channel's brief description
    bool m_bAvail; // is this channel available ?

    QString m_ReadAlias(QString& sInput);
    QString m_ReadChannelStatus(QString& sInput);
};

#endif // FINCHANNELINTERFACE_H
