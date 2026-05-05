#ifndef SHEADINTERFACE_H
#define SHEADINTERFACE_H

#include "resource.h"
#include "scinchannelinterface.h"
#include "scinsettings.h"
#include <scpi.h>
#include <QList>

class ScInGroupResourceAndInterface : public cResource
{
    Q_OBJECT
public:
    const QString Version = "V1.00";
    ScInGroupResourceAndInterface(const std::shared_ptr<cSCPI> &scpiInterface,
                                  ScInSettings *settings);
    ~ScInGroupResourceAndInterface();
    void initSCPIConnection() override;
    void registerResource(RMConnection *rmConnection, quint16 port) override;
protected:
    void executeProtoScpi(int cmdCode, const ProtonetCommandPtr &protoCmd) override;
private:
    QString scpiReadVersion(const QString& scpi);
    QString readChannelCatalog(const QString& scpi);
    QList<ScInChannelInterface*> m_ChannelList;
};

#endif // SHEADINTERFACE_H
