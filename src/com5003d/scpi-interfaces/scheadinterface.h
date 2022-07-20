#ifndef SHEADINTERFACE_H
#define SHEADINTERFACE_H


#include <QObject>
#include <QList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

#include "resource.h"
#include <scpiconnection.h>
#include "scheadchannel.h"


namespace SCHeadSystem
{

const QString Version = "V1.00";

enum Commands
{
    cmdVersion,
    cmdChannelCat
};
}

class cCOM5003dServer;
class cSCHeadSettings;


class cSCHeadInterface : public cResource
{
    Q_OBJECT

public:
    cSCHeadInterface(cCOM5003dServer* server);
    ~cSCHeadInterface();
    virtual void initSCPIConnection(QString leadingNodes);
    virtual void registerResource(cRMConnection *rmConnection, quint16 port);
    virtual void unregisterResource(cRMConnection *rmConnection);

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd);

private:
    cCOM5003dServer* m_pMyServer;
    QList<cSCHeadChannel*> m_ChannelList;
    QString m_sVersion;

    QString m_ReadVersion(QString& sInput);
    QString m_ReadChannelCatalog(QString& sInput);

};


#endif // SHEADINTERFACE_H
