#ifndef HKINGROUPRESOURCEANDINTERFACE_H
#define HKINGROUPRESOURCEANDINTERFACE_H

#include "resource.h"
#include "hkinchannelinterface.h"
#include "hkinsettings.h"
#include <scpi.h>
#include <QList>

class HkInGroupResourceAndInterface : public cResource
{
    Q_OBJECT
public:
    const QString Version = "V1.00";
    HkInGroupResourceAndInterface(std::shared_ptr<cSCPI> scpiInterface,
                                  HkInSettings *settings);
    ~HkInGroupResourceAndInterface();
    void initSCPIConnection(QString leadingNodes) override;
    void registerResource(RMConnection *rmConnection, quint16 port) override;
protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
private:
    QString readVersion(QString& sInput);
    QString readChannelCatalog(QString& sInput);
    QList<HkInChannelInterface*> m_ChannelList;
};

#endif // HKINGROUPRESOURCEANDINTERFACE_H
