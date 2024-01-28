#ifndef SENSEINTERFACE_H
#define SENSEINTERFACE_H

#include "adjustmenteeprom.h"
#include "ethsettings.h"
#include "senseinterfacecommon.h"
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
    };

    const QString sVoltageChannelDescription = "Measuring channel 0..480V AC";
    const QString sCurrentChannelDescription = "Measuring channel 0..160A AC";
    const QString sMeasuringModeDescription = "Measuring mode switch AC,REF";
    const QString sReferenceChannelDescription = "Reference channel 0..10V DC";
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
    virtual void initSCPIConnection(QString leadingNodes) override;
    SenseChannelCommon* getChannel(QString& name);

    QString exportXMLString(int indent = 1) override;

    void registerResource(RMConnection *rmConnection, quint16 port) override;

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
    QString m_ReadMModeCatalog(QString& scpi);
    QString m_ReadSenseChannelCatalog(QString& sInput);
    QString m_ReadSenseGroupCatalog(QString& sInput);
    QString m_InitSenseAdjData(QString& sInput);
    QString m_ComputeSenseAdjData(QString& sInput);
    QString scpiReadAdjStatus(QString& sInput);

    // COM specifics
    RangeAdjInterface* createJustScpiInterfaceWithAtmelPermission() override;
    void scpiReadWriteMMode(cProtonetCommand* protoCmd) override;
    enum SetModeModeResult {
        done,
        start_statemachine_required,
        failed
    };
    SetModeModeResult setSenseMode(QString mode);
    RMConnection* m_rmConnection;
    EthSettings* m_ethSettings;

    QStateMachine m_ChangeSenseModeMachine;
    QState m_UnregisterSenseState;
    QState m_RegisterSenseState;
    QFinalState m_NotifySenseState;
    QList<cProtonetCommand*> sensemodeProtonetCmdList;
};

#endif // SENSEINTERFACE_H
