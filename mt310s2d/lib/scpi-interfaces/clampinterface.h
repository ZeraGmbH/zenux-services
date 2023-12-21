#ifndef CLAMPINTERFACE
#define CLAMPINTERFACE

#include "scpiconnection.h"
#include "pcbserver.h"
#include "notificationstring.h"
#include "mt310s2senseinterface.h"
#include <QHash>
#include <QSet>

// here we hold the clamps that are hotplugged to the system

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
    cClampInterface(cPCBServer *server, cI2CSettings *i2cSettings, cSenseSettings *senseSettings, Mt310s2SenseInterface *senseInterface);
    virtual void initSCPIConnection(QString leadingNodes) override;
    void actualizeClampStatus(quint16 devConnectedMask);
    // lazy: public for test
    void addClamp(QString channelName, int ctrlChannel, I2cMuxerInterface::Ptr i2cMuxer, quint16 bmask, int phaseCount);
    void removeClamp(QString channelName, const SenseSystem::cChannelSettings *chSettings, quint16 bmask);
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    void generateAndNotifyClampChannelList();
    QString readClampChannelCatalog(QString& sInput);
    QString writeAllClamps(QString& sInput);
    QString importExportAllClamps(QString& sInput);
    void handleClampConnected(QString channelName, const SenseSystem::cChannelSettings *chSettings, quint16 bmask, int phaseCount);

    cPCBServer *m_pMyServer;
    cI2CSettings *m_i2cSettings;
    cSenseSettings *m_senseSettings;
    Mt310s2SenseInterface *m_pSenseInterface;
    NotificationString m_notifierClampChannelList;
    quint16 m_nClampStatus;
    QHash<QString, cClamp*> m_clampHash;
    QSet<QString> m_clampSecondarySet;
};

#endif // CLAMPINTERFACE

