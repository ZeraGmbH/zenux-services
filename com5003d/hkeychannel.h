#ifndef HKEYCHANNEL_H
#define HKEYCHANNEL_H

#include <QObject>

#include "scpiconnection.h"

namespace HKEYChannel
{
enum Commands
{
    cmdAlias,
    cmdStatus
};

}


namespace HKeySystem
{
    class cChannelSettings;
}

class cSCPI;

class cHKeyChannel : public ScpiConnection
{
    Q_OBJECT

public:
    cHKeyChannel(cSCPI* scpiinterface, QString description, quint8 nr, HKeySystem::cChannelSettings* cSettings);
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

#endif // HKEYCHANNEL_H
