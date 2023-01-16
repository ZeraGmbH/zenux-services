#ifndef FINGROUPRESOURCEANDINTERFACE_H
#define FINGROUPRESOURCEANDINTERFACE_H

#include "resource.h"
#include "scpiconnection.h"
#include "finchannelinterface.h"
#include "frqinputsettings.h"
#include <scpi.h>
#include <QObject>
#include <QList>

namespace FRQInputSystem
{
    const QString Version = "V1.00";
    enum Commands
    {
        cmdVersion,
        cmdChannelCat
    };
}

class FInGroupResourceAndInterface : public cResource
{
    Q_OBJECT
public:
    FInGroupResourceAndInterface(cSCPI *scpiInterface, cFRQInputSettings *settings);
    ~FInGroupResourceAndInterface();
    virtual void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    virtual void unregisterResource(RMConnection *rmConnection) override;
protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString m_ReadVersion(QString& sInput);
    QString m_ReadChannelCatalog(QString& sInput);
    QList<FInChannelInterface*> m_ChannelList;
    QString m_sVersion;
};

#endif // FINGROUPRESOURCEANDINTERFACE_H
