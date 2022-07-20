#ifndef CLAMPINTERFACE
#define CLAMPINTERFACE

#include <scpiconnection.h>
#include <notificationstring.h>
#include "senseinterface.h"
#include <QHash>
#include <QSet>

// here we hold the clamps that are hotplugged to the system

class cMT310S2dServer;
class cClamp;

namespace ClampSystem
{
enum ClampCommands
{
    cmdClampChannelCat,
    cmdClampWrite,
    cmdClampImportExport
};
}

class cClampInterface: public cSCPIConnection
{
public:
    cClampInterface(cMT310S2dServer *server);
    virtual void initSCPIConnection(QString leadingNodes);
    void actualizeClampStatus(quint16 devConnectedMask);

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd);
private:
    void generateAndNotifyClampChannelList();
    QString readClampChannelCatalog(QString& sInput);
    QString writeAllClamps(QString& sInput);
    QString importExportAllClamps(QString& sInput);

    cMT310S2dServer *m_pMyServer;
    cSenseInterface *m_pSenseInterface;
    cNotificationString m_notifierClampChannelList;
    quint16 m_nClampStatus;
    QHash<QString, cClamp*> m_clampHash;
    QSet<QString> m_clampSecondarySet;
};

#endif // CLAMPINTERFACE

