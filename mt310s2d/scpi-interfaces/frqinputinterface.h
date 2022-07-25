#ifndef FRQINPUTINTERFACE_H
#define FRQINPUTINTERFACE_H

#include <QObject>
#include <QList>
#include "resource.h"
#include <scpiconnection.h>
#include "fpzinchannel.h"

namespace FRQInputSystem
{
    const QString Version = "V1.00";
    enum Commands
    {
        cmdVersion,
        cmdChannelCat
    };
}

class cFRQInputInterface : public cResource
{
    Q_OBJECT
public:
    cFRQInputInterface(cMT310S2dServer *server);
    ~cFRQInputInterface();
    virtual void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    virtual void unregisterResource(RMConnection *rmConnection) override;
protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString m_ReadVersion(QString& sInput);
    QString m_ReadChannelCatalog(QString& sInput);

    cMT310S2dServer* m_pMyServer;
    QList<cFPZInChannel*> m_ChannelList;
    QString m_sVersion;

};

#endif // FRQINPUTINTERFACE_H
