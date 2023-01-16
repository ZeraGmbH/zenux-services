#ifndef HKINGROUPRESOURCEANDINTERFACE_H
#define HKINGROUPRESOURCEANDINTERFACE_H

#include "resource.h"
#include "scpiconnection.h"
#include "hkinchannelinterface.h"
#include "hkinsettings.h"
#include <scpi.h>
#include <QList>

class HkInGroupResourceAndInterface : public cResource
{
    Q_OBJECT
public:
    const QString Version = "V1.00";
    HkInGroupResourceAndInterface(cSCPI *scpiInterface, HkInSettings *settings);
    ~HkInGroupResourceAndInterface();
    virtual void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    virtual void unregisterResource(RMConnection *rmConnection) override;
protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString readVersion(QString& sInput);
    QString readChannelCatalog(QString& sInput);
    QList<HkInChannelInterface*> m_ChannelList;
};

#endif // HKINGROUPRESOURCEANDINTERFACE_H
