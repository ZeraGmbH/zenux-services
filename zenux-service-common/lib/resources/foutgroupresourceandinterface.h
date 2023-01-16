#ifndef FOUTGROUPRESOURCEANDINTERFACE_H
#define FOUTGROUPRESOURCEANDINTERFACE_H

#include "foutchannelinterface.h"
#include "resource.h"
#include "sourcesettings.h"
#include <scpi.h>
#include <QObject>
#include <QList>

class FOutGroupResourceAndInterface : public cResource
{
    Q_OBJECT
public:
    const QString Version = "V1.00";
    enum Commands
    {
        cmdVersion,
        cmdChannelCat
    };
    FOutGroupResourceAndInterface(cSCPI *scpiInterface, cSourceSettings* settings);
    ~FOutGroupResourceAndInterface();
    virtual void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    virtual void unregisterResource(RMConnection *rmConnection) override;
protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString readVersion(QString &sInput);
    QString readSourceChannelCatalog(QString &sInput);
    QList<FOutChannelInterface*> m_ChannelList;
};

#endif // FOUTGROUPRESOURCEANDINTERFACE_H
