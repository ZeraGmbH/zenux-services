#ifndef SENSEINTERFACE_H
#define SENSEINTERFACE_H

#include "adjustmenteeprom.h"
#include "ethsettings.h"
#include "senseinterfacecommon.h"
#include <QList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

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
// COM specifics only
    int rangeFlagsDevice() override;
    int rangeFlagsIntern() override;
    int rangeFlagsExtern() override;
    int rangeFlagsExternDc() override;

protected:
    void exportAdjData(QDataStream& stream, QDateTime dateTimeWrite) override;

private slots:
    void unregisterSense();
    void registerSense();
    void notifySense();
private:
    QString getServerVersion() override;
    QString getPcbName() override;
    QString getXmlType() override;
    bool isRangePartOfAdjXmlExport(SenseRangeCommon* range) override;
    RangeAdjInterface* createJustScpiInterfaceWithAtmelPermission() override;
    QString scpiReadSenseGroupCatalog(QString& scpi) override;
    void handleScpiReadWriteMMode(cProtonetCommand* protoCmd) override;
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
