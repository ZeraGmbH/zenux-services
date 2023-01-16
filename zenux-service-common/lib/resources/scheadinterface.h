#ifndef SHEADINTERFACE_H
#define SHEADINTERFACE_H

#include "resource.h"
#include "scpiconnection.h"
#include "scinchannelinterface.h"
#include "scinsettings.h"
#include <scpi.h>
#include <QList>

namespace SCHeadSystem
{

const QString Version = "V1.00";

}

class cSCHeadInterface : public cResource
{
    Q_OBJECT
public:
    cSCHeadInterface(cSCPI *scpiInterface, ScInSettings *settings);
    ~cSCHeadInterface();
    virtual void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    virtual void unregisterResource(RMConnection *rmConnection) override;
protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QList<ScInChannelInterface*> m_ChannelList;
    QString m_sVersion;

    QString m_ReadVersion(QString& sInput);
    QString m_ReadChannelCatalog(QString& sInput);
};

#endif // SHEADINTERFACE_H
