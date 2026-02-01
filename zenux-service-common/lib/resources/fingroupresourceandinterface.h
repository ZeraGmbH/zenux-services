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
    void initSCPIConnection() override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
private:
    QString readVersion(QString& sInput);
    QString readChannelCatalog(QString& sInput);
    QList<FInChannelInterface*> m_ChannelList;
};

#endif // FINGROUPRESOURCEANDINTERFACE_H
