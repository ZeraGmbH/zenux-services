#ifndef FINGROUPRESOURCEANDINTERFACE_H
#define FINGROUPRESOURCEANDINTERFACE_H

#include "resource.h"
#include "scpiconnection.h"
#include "finchannelinterface.h"
#include "finsettings.h"
#include <scpi.h>
#include <QObject>
#include <QList>

class FInGroupResourceAndInterface : public cResource
{
    Q_OBJECT
public:
    const QString Version = "V1.00";
    FInGroupResourceAndInterface(cSCPI *scpiInterface, FInSettings *settings);
    ~FInGroupResourceAndInterface();
    virtual void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    virtual void unregisterResource(RMConnection *rmConnection) override;
protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString readVersion(QString& sInput);
    QString readChannelCatalog(QString& sInput);
    QList<FInChannelInterface*> m_ChannelList;
};

#endif // FINGROUPRESOURCEANDINTERFACE_H
