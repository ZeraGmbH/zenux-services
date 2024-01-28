#ifndef SENSEINTERFACE_H
#define SENSEINTERFACE_H

#include "adjustmenteeprom.h"
#include "adjustmentxmlimportexporttemplate.h"
#include "i2csettings.h"
#include "ethsettings.h"
#include "sensechannelcommon.h"
#include "senseinterfacecommon.h"
#include "systeminfo.h"
#include <QList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace SenseSystem
{
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

class Com5003SenseInterface : public SenseInterfaceCommon
{
    Q_OBJECT
public:
    Com5003SenseInterface(cSCPI *scpiInterface,
                          I2cSettings* i2cSettings,
                          RMConnection* rmConnection,
                          EthSettings* ethSettings,
                          cSenseSettings* senseSettings,
                          SystemInfo *systemInfo,
                          AbstractFactoryI2cCtrlPtr ctrlFactory);
    ~Com5003SenseInterface();
    virtual void initSCPIConnection(QString leadingNodes) override;
    SenseChannelCommon* getChannel(QString& name);
    quint8 getAdjustmentStatus() override;

    QString exportXMLString(int indent = 1) override;

    void registerResource(RMConnection *rmConnection, quint16 port) override;
    void computeSenseAdjData();

protected:
    void exportAdjData(QDataStream& stream, QDateTime dateTimeWrite) override;
    bool importAdjData(QDataStream& stream) override;
    bool importXMLDocument(QDomDocument* qdomdoc) override;
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;

private slots:
    void unregisterSense();
    void registerSense();
    void notifySense();

private:
    QString scpiReadVersion(QString& sInput);
    void scpiReadWriteMMode(cProtonetCommand* protoCmd);
    QString m_ReadMModeCatalog(QString& sInput);
    QString m_ReadSenseChannelCatalog(QString& sInput);
    QString m_ReadSenseGroupCatalog(QString& sInput);
    QString m_InitSenseAdjData(QString& sInput);
    QString m_ComputeSenseAdjData(QString& sInput);
    QString scpiReadAdjStatus(QString& sInput);
    RangeAdjInterface* createJustScpiInterfaceWithAtmelPermission();
    void setNotifierSenseMMode();
    void setNotifierSenseChannelCat();

    SystemInfo *m_systemInfo;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;

    QList<SenseChannelCommon*> m_ChannelList;
    QString m_sVersion;
    quint8 m_nMMode;

    quint8 m_nSerialStatus;

    NotificationString notifierSenseMMode;
    NotificationString notifierSenseChannelCat;

    // COM specifics
    RMConnection* m_rmConnection;
    EthSettings* m_ethSettings;

    QHash<QString,quint8> m_MModeHash;
    QStateMachine m_ChangeSenseModeMachine;
    QState m_UnregisterSenseState;
    QState m_RegisterSenseState;
    QFinalState m_NotifySenseState;
    QList<cProtonetCommand*> sensemodeProtonetCmdList;
};

#endif // SENSEINTERFACE_H
