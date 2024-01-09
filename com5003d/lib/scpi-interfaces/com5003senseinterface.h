#ifndef SENSEINTERFACE_H
#define SENSEINTERFACE_H

#include "adjustmenteeprom.h"
#include "adjustmentxmlimportexporttemplate.h"
#include "i2csettings.h"
#include "resource.h"
#include "ethsettings.h"
#include "com5003sensechannel.h"
#include "notificationstring.h"
#include "systeminfo.h"
#include <QList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace SenseSystem
{

const QString Version = "V1.00";

enum Commands
{
    cmdVersion,
    cmdMMode,
    cmdMModeCat,
    cmdChannelCat,
    cmdGroupCat,
    initAdjData,
    computeAdjData
};

enum MMode
{
    modeAC,
    modeREF,
    modeAnz
};

const QString sVoltageChannelDescription = "Measuring channel 0..480V AC";
const QString sCurrentChannelDescription = "Measuring channel 0..160A AC";
const QString sMeasuringModeDescription = "Measuring mode switch AC,REF";

const QString sReferenceChannelDescription = "Reference channel 0..10V DC";
const QString sMMode[2] = {"AC", "REF"};
}

class Com5003SenseInterface : public cResource, public AdjustmentEeprom, public AdjustmentXmlImportExportTemplate
{
    Q_OBJECT
public:
    Com5003SenseInterface(cSCPI *scpiInterface,
                          cI2CSettings* i2cSettings,
                          RMConnection* rmConnection,
                          EthSettings* ethSettings,
                          cSenseSettings* senseSettings,
                          cSystemInfo *systemInfo,
                          AtmelPermissionTemplate *permissionQueryHandler);
    ~Com5003SenseInterface();
    virtual void initSCPIConnection(QString leadingNodes) override;
    Com5003SenseChannel* getChannel(QString& name);
    quint8 getAdjustmentStatus() override; // we return 0 if not adj. else 1
    virtual void exportAdjData(QDataStream& stream, QDateTime dateTimeWrite) override;
    virtual bool importAdjData(QDataStream& stream) override;

    QString exportXMLString(int indent = 1) override;

    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    void computeSenseAdjData();

protected:
    bool importXMLDocument(QDomDocument* qdomdoc) override;
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;

private slots:
    void unregisterSense();
    void registerSense();
    void notifySense();

private:
    RMConnection* m_rmConnection;
    EthSettings* m_ethSettings;
    cSystemInfo *m_systemInfo;
    AtmelPermissionTemplate *m_permissionQueryHandler;
    QList<Com5003SenseChannel*> m_ChannelList;
    QString m_sVersion;
    quint8 m_nMMode;

    QStateMachine m_ChangeSenseModeMachine;
    QState m_UnregisterSenseState;
    QState m_RegisterSenseState;
    QFinalState m_NotifySenseState;
    QList<cProtonetCommand*> sensemodeProtonetCmdList;

    QString scpiReadVersion(QString& sInput);
    void scpiReadWriteMMode(cProtonetCommand* protoCmd);
    QString m_ReadMModeCatalog(QString& sInput);
    QString m_ReadSenseChannelCatalog(QString& sInput);
    QString m_ReadSenseGroupCatalog(QString& sInput);
    QString m_InitSenseAdjData(QString& sInput);
    QString m_ComputeSenseAdjData(QString& sInput);
    RangeAdjInterface* createJustScpiInterfaceWithAtmelPermission();
    void setNotifierSenseMMode();
    void setNotifierSenseChannelCat();

    quint8 m_nSerialStatus;

    NotificationString notifierSenseMMode;
    NotificationString notifierSenseChannelCat;
};

#endif // SENSEINTERFACE_H
