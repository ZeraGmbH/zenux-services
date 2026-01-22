#ifndef FINGROUPRESOURCEANDINTERFACE_H
#define FINGROUPRESOURCEANDINTERFACE_H

#include "resource.h"
#include "finchannelinterface.h"
#include "finsettings.h"
#include <scpi.h>
#include <QList>

class FInGroupResourceAndInterface : public cResource
{
    Q_OBJECT
public:
    const QString Version = "V1.00";
    FInGroupResourceAndInterface(std::shared_ptr<cSCPI> scpiInterface,
                                 FInSettings *settings);
    ~FInGroupResourceAndInterface();
    virtual void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
private:
    QString readVersion(const QString& sInput);
    QString readChannelCatalog(const QString& sInput);
    QList<FInChannelInterface*> m_ChannelList;
};

#endif // FINGROUPRESOURCEANDINTERFACE_H
