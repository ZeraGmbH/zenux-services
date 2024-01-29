#ifndef CLAMPINTERFACE
#define CLAMPINTERFACE

#include "scpiconnection.h"
#include "pcbserver.h"
#include "notificationstring.h"
#include "senseinterfacecommon.h"
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
    cClampInterface(cPCBServer *server,
                    I2cSettings *i2cSettings,
                    cSenseSettings *senseSettings,
                    SenseInterfaceCommon *senseInterface,
                    AbstractFactoryI2cCtrlPtr ctrlFactory);
    virtual void initSCPIConnection(QString leadingNodes) override;
    void actualizeClampStatus(quint16 devConnectedMask);
    // lazy: public for test
    void addClamp(const SenseSystem::cChannelSettings *chSettings, I2cMuxerInterface::Ptr i2cMuxer);
    QString exportXMLString(int indent = 1);
    QString importClampXmls(QString allXML, bool computeAndExport);

protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    void generateAndNotifyClampChannelList();
    QString readClampChannelCatalog(QString& sInput);
    QString writeAllClamps(QString& sInput);
    QString importExportAllClamps(QString& sInput);
    void handleClampConnected(const SenseSystem::cChannelSettings *chSettings);
    void handleClampDisconnected(QString channelName, const SenseSystem::cChannelSettings *chSettings, quint16 bmask);

    cPCBServer *m_pMyServer;
    I2cSettings *m_i2cSettings;
    cSenseSettings *m_senseSettings;
    SenseInterfaceCommon *m_pSenseInterface;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    NotificationString m_notifierClampChannelList;
    quint16 m_nClampStatus;
    QHash<QString, cClamp*> m_clampHash;
    QSet<QString> m_clampSecondarySet;
};

#endif // CLAMPINTERFACE

