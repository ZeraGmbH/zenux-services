#ifndef SCHEADCHANNEL_H
#define SCHEADCHANNEL_H

#include "scpiconnection.h"
#include "scinsettings.h"
#include <QObject>

namespace SCHEADChannel
{
enum Commands
{
    cmdAlias,
    cmdStatus
};

}

class cSCHeadChannel : public ScpiConnection
{
    Q_OBJECT
public:
    cSCHeadChannel(cSCPI* scpiinterface, QString description, quint8 nr, ScInSettings::ChannelSettings* cSettings);
    virtual void initSCPIConnection(QString leadingNodes) override;

    QString& getName();
    QString& getAlias();
    QString& getDescription();
    bool isAvail();
protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString m_ReadAlias(QString& sInput);
    QString m_ReadChannelStatus(QString& sInput);
    QString m_sName; // the channel's name
    QString m_sAlias;
    QString m_sDescription; // the channel's brief description
    bool m_bAvail; // is this channel available ?
};


#endif // SCHEADCHANNEL_H
