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
class cCOM5003dServer;

class cSourceInterface : public cResource
{
    Q_OBJECT

public:
    cSourceInterface(cCOM5003dServer* server);
    ~cSourceInterface();
    virtual void initSCPIConnection(QString leadingNodes);
    virtual void registerResource(cRMConnection *rmConnection, quint16 port);
    virtual void unregisterResource(cRMConnection *rmConnection);

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd);

private:
    cCOM5003dServer* m_pMyServer;
    QList<cFPZChannel*> m_ChannelList;
    QString m_sVersion;

    QString m_ReadVersion(QString &sInput);
    QString m_ReadSourceChannelCatalog(QString &sInput);
};



#endif // SOURCEINTERFACE_H
