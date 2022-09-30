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
    virtual void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    virtual void unregisterResource(RMConnection *rmConnection) override;

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    cMT310S2dServer* m_pMyServer;
    QList<cHKeyChannel*> m_ChannelList;
    QString m_sVersion;

    QString m_ReadVersion(QString& sInput);
    QString m_ReadChannelCatalog(QString& sInput);

};


#endif // HKEYINTERFACE_H
