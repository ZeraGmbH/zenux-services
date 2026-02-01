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
    ScInGroupResourceAndInterface(std::shared_ptr<cSCPI> scpiInterface,
                                  ScInSettings *settings);
    ~ScInGroupResourceAndInterface();
    void initSCPIConnection(const QString &leadingNodes) override;
    void registerResource(RMConnection *rmConnection, quint16 port) override;
protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
private:
    QString scpiReadVersion(QString& sInput);
    QString m_ReadChannelCatalog(QString& sInput);
    QList<ScInChannelInterface*> m_ChannelList;
};

#endif // SHEADINTERFACE_H
