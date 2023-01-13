#ifndef SHEADINTERFACE_H
#define SHEADINTERFACE_H

#include "resource.h"
#include "scpiconnection.h"
#include "scheadchannel.h"
#include <QObject>
#include <QList>

namespace SCHeadSystem
{

const QString Version = "V1.00";

enum Commands
{
    cmdVersion,
    cmdChannelCat
};
}

class cMT310S2dServer;
class cSCHeadSettings;


class cSCHeadInterface : public cResource
{
    Q_OBJECT

public:
    cSCHeadInterface(cMT310S2dServer *server);
    ~cSCHeadInterface();
    virtual void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    virtual void unregisterResource(RMConnection *rmConnection) override;

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    cMT310S2dServer* m_pMyServer;
    QList<cSCHeadChannel*> m_ChannelList;
    QString m_sVersion;

    QString m_ReadVersion(QString& sInput);
    QString m_ReadChannelCatalog(QString& sInput);

};


#endif // SHEADINTERFACE_H
