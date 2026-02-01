#ifndef FOUTGROUPRESOURCEANDINTERFACE_H
#define FOUTGROUPRESOURCEANDINTERFACE_H

#include "foutchannelinterface.h"
#include "resource.h"
#include "foutsettings.h"
#include <scpi.h>
#include <QList>

class FOutGroupResourceAndInterface : public cResource
{
    Q_OBJECT
public:
    const QString Version = "V1.00";
    FOutGroupResourceAndInterface(std::shared_ptr<cSCPI> scpiInterface,
                                  FOutSettings* settings);
    ~FOutGroupResourceAndInterface();
    void initSCPIConnection() override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
private:
    QString readVersion(QString &sInput);
    QString readSourceChannelCatalog(QString &sInput);
    QList<FOutChannelInterface*> m_ChannelList;
};

#endif // FOUTGROUPRESOURCEANDINTERFACE_H
