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
    ScInGroupResourceAndInterface(cSCPI *scpiInterface, ScInSettings *settings);
    ~ScInGroupResourceAndInterface();
    void initSCPIConnection(QString leadingNodes) override;
    void registerResource(RMConnection *rmConnection, quint16 port) override;
    void unregisterResource(RMConnection *rmConnection) override;
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString m_ReadVersion(QString& sInput);
    QString m_ReadChannelCatalog(QString& sInput);
    QList<ScInChannelInterface*> m_ChannelList;
};

#endif // SHEADINTERFACE_H
