#ifndef SOURCEINTERFACE_H
#define SOURCEINTERFACE_H

#include "fpzchannel.h"
#include "resource.h"
#include "sourcesettings.h"
#include <scpi.h>
#include <QObject>
#include <QList>

namespace SourceSystem
{
const QString Version = "V1.00";

enum Commands
{
    cmdVersion,
    cmdChannelCat
};
}

class cSourceInterface : public cResource
{
    Q_OBJECT
public:
    cSourceInterface(cSCPI *scpiInterface, cSourceSettings* settings);
    ~cSourceInterface();
    virtual void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    virtual void unregisterResource(RMConnection *rmConnection) override;

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    QList<cFPZChannel*> m_ChannelList;
    QString m_sVersion;

    QString m_ReadVersion(QString &sInput);
    QString m_ReadSourceChannelCatalog(QString &sInput);
};



#endif // SOURCEINTERFACE_H
