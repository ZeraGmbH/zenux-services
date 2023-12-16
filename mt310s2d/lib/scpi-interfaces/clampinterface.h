#ifndef CLAMPINTERFACE
#define CLAMPINTERFACE

#include "scpiconnection.h"
#include "notificationstring.h"
#include "mt310s2senseinterface.h"
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

class cClampInterface: public ScpiConnection
{
public:
    cClampInterface(cMT310S2dServer *server);
    virtual void initSCPIConnection(QString leadingNodes) override;
    void actualizeClampStatus(quint16 devConnectedMask);
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    void generateAndNotifyClampChannelList();
    QString readClampChannelCatalog(QString& sInput);
    QString writeAllClamps(QString& sInput);
    QString importExportAllClamps(QString& sInput);
    void handleClampDisconnected(QString channelName, const SenseSystem::cChannelSettings *chSettings, quint16 bmask);
    void handleClampConnected(QString channelName, const SenseSystem::cChannelSettings *chSettings, quint16 bmask, int phaseCount);

    cMT310S2dServer *m_pMyServer;
    Mt310s2SenseInterface *m_pSenseInterface;
    NotificationString m_notifierClampChannelList;
    quint16 m_nClampStatus;
    QHash<QString, cClamp*> m_clampHash;
    QSet<QString> m_clampSecondarySet;
};

#endif // CLAMPINTERFACE

