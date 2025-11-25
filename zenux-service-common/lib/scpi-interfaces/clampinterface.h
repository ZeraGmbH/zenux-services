#ifndef CLAMPINTERFACE
#define CLAMPINTERFACE

#include "abstracteepromi2cfactory.h"
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
    cClampInterface(PCBServer *server,
                    cSenseSettings *senseSettings,
                    SenseInterfaceCommon *senseInterface,
                    I2cSettings *i2cSettings,
                    AbstractEepromI2cFactoryPtr adjMemFactory,
                    AbstractFactoryI2cCtrlPtr ctrlFactory);
    virtual void initSCPIConnection(QString leadingNodes) override;
    void actualizeClampStatus(quint16 devConnectedMask);
    // lazy: public for test
    cClamp *tryAddClamp(const SenseSystem::cChannelSettings *chSettings);
    cClamp *addClamp(const SenseSystem::cChannelSettings *chSettings, EepromI2cDeviceInterfacePtr adjMemory);
    QString exportXMLString(int indent = 1);
    QString importClampXmls(QString allXML, bool computeAndExport);

protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;

private:
    void generateAndNotifyClampChannelList();
    QString readClampChannelCatalog(QString& sInput);
    QString writeAllClamps(QString& sInput);
    QString importExportAllClamps(QString& sInput);
    void handleClampDisconnected(QString channelName, const SenseSystem::cChannelSettings *chSettings, quint16 bmask);

    PCBServer *m_pMyServer;
    cSenseSettings *m_senseSettings;
    SenseInterfaceCommon *m_pSenseInterface;
    I2cSettings *m_i2cSettings;
    AbstractEepromI2cFactoryPtr m_adjMemFactory;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    NotificationString m_notifierClampChannelList;
    quint16 m_nClampStatus;
    QHash<QString, cClamp*> m_clampHash;
    QSet<QString> m_clampSecondarySet;
};

#endif // CLAMPINTERFACE

