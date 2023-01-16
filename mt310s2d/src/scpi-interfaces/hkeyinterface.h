#ifndef HKEYINTERFACE_H
#define HKEYINTERFACE_H

#include "resource.h"
#include "scpiconnection.h"
#include "hkeychannel.h"
#include "hkinsettings.h"
#include <scpi.h>
#include <QList>

namespace HKeySystem
{

const QString Version = "V1.00";

enum Commands
{
    cmdVersion,
    cmdChannelCat
};
}

class cHKeyInterface : public cResource
{
    Q_OBJECT

public:
    cHKeyInterface(cSCPI *scpiInterface, HkInSettings *settings);
    ~cHKeyInterface();
    virtual void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    virtual void unregisterResource(RMConnection *rmConnection) override;

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    QList<cHKeyChannel*> m_ChannelList;
    QString m_sVersion;

    QString m_ReadVersion(QString& sInput);
    QString m_ReadChannelCatalog(QString& sInput);

};


#endif // HKEYINTERFACE_H
