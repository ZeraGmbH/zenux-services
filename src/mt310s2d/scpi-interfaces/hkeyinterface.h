#ifndef HKEYINTERFACE_H
#define HKEYINTERFACE_H

#include <QObject>
#include <QList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

#include "resource.h"
#include <scpiconnection.h>
#include "hkeychannel.h"


namespace HKeySystem
{

const QString Version = "V1.00";

enum Commands
{
    cmdVersion,
    cmdChannelCat
};
}

class cMT310S2dServer;

class cHKeyInterface : public cResource
{
    Q_OBJECT

public:
    cHKeyInterface(cMT310S2dServer *server);
    ~cHKeyInterface();
    virtual void initSCPIConnection(QString leadingNodes);
    virtual void registerResource(cRMConnection *rmConnection, quint16 port);
    virtual void unregisterResource(cRMConnection *rmConnection);

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd);

private:
    cMT310S2dServer* m_pMyServer;
    QList<cHKeyChannel*> m_ChannelList;
    QString m_sVersion;

    QString m_ReadVersion(QString& sInput);
    QString m_ReadChannelCatalog(QString& sInput);

};


#endif // HKEYINTERFACE_H
