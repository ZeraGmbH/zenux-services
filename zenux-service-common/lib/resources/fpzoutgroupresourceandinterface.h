#ifndef FPZOUTGROUPRESOURCEANDINTERFACE_H
#define FPZOUTGROUPRESOURCEANDINTERFACE_H

#include "fpzoutchannelinterface.h"
#include "resource.h"
#include "sourcesettings.h"
#include <scpi.h>
#include <QObject>
#include <QList>

class FpzOutGroupResourceAndInterface : public cResource
{
    Q_OBJECT
public:
    const QString Version = "V1.00";
    enum Commands
    {
        cmdVersion,
        cmdChannelCat
    };
    FpzOutGroupResourceAndInterface(cSCPI *scpiInterface, cSourceSettings* settings);
    ~FpzOutGroupResourceAndInterface();
    virtual void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    virtual void unregisterResource(RMConnection *rmConnection) override;
protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString readVersion(QString &sInput);
    QString readSourceChannelCatalog(QString &sInput);
    QList<FpzOutChannelInterface*> m_ChannelList;
};

#endif // FPZOUTGROUPRESOURCEANDINTERFACE_H
