#ifndef SCHEADCHANNEL_H
#define SCHEADCHANNEL_H

#include <QObject>

#include <scpiconnection.h>

namespace SCHEADChannel
{
enum Commands
{
    cmdAlias,
    cmdStatus
};

}


namespace SCHeadSystem
{
    class cChannelSettings;
}

class cSCPI;

class cSCHeadChannel : public cSCPIConnection
{
    Q_OBJECT

public:
    cSCHeadChannel(cSCPI* scpiinterface, QString description, quint8 nr, SCHeadSystem::cChannelSettings* cSettings);
    virtual void initSCPIConnection(QString leadingNodes);

    QString& getName();
    QString& getAlias();
    QString& getDescription();
    bool isAvail();

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd);

private:
    QString m_sName; // the channel's name
    QString m_sAlias;
    QString m_sDescription; // the channel's brief description
    bool m_bAvail; // is this channel available ?

    QString m_ReadAlias(QString& sInput);
    QString m_ReadChannelStatus(QString& sInput);
};


#endif // SCHEADCHANNEL_H
