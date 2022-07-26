#ifndef SOURCEINTERFACE_H
#define SOURCEINTERFACE_H


#include <QObject>
#include <QList>

#include "fpzchannel.h"
#include "resource.h"

namespace SourceSystem
{

const QString Version = "V1.00";

enum Commands
{
    cmdVersion,
    cmdChannelCat
};
}


class cSourceSettings;
class QDataStream;
class cMT310S2dServer;

class cSourceInterface : public cResource
{
    Q_OBJECT

public:
    cSourceInterface(cMT310S2dServer *server);
    ~cSourceInterface();
    virtual void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    virtual void unregisterResource(RMConnection *rmConnection) override;

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    cMT310S2dServer* m_pMyServer;
    QList<cFPZChannel*> m_ChannelList;
    QString m_sVersion;

    QString m_ReadVersion(QString &sInput);
    QString m_ReadSourceChannelCatalog(QString &sInput);
};



#endif // SOURCEINTERFACE_H
