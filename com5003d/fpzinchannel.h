#ifndef FPZINCHANNEL_H
#define FPZINCHANNEL_H

#include <QObject>

#include <scpiconnection.h>

namespace FPZINChannel
{
enum Commands
{
    cmdAlias,
    cmdStatus
};

}


namespace FRQInputSystem
{
    class cChannelSettings;
}

class cCOM5003dServer;

class cFPZInChannel : public cSCPIConnection
{
    Q_OBJECT

public:
    cFPZInChannel(cCOM5003dServer* server, QString description, quint8 nr, FRQInputSystem::cChannelSettings* cSettings);
    virtual void initSCPIConnection(QString leadingNodes);

    QString& getName();
    QString& getAlias();
    QString& getDescription();
    bool isAvail();

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd);

private:
    cCOM5003dServer* m_pMyServer;
    QString m_sName; // the channel's name
    QString m_sAlias;
    QString m_sDescription; // the channel's brief description
    bool m_bAvail; // is this channel available ?

    QString m_ReadAlias(QString& sInput);
    QString m_ReadChannelStatus(QString& sInput);
};

#endif // FPZINCHANNEL_H
