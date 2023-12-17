#ifndef SENSEINTERFACE_H
#define SENSEINTERFACE_H

#include "i2csettings.h"
#include "mt310s2adjflash.h"
#include "mt310s2adjxml.h"
#include "mt310s2systeminfo.h"
#include "resource.h"
#include "mt310s2sensechannel.h"
#include "systeminfo.h"
#include <QList>
#include <QHash>

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
        computeAdjData,
        cmdStatAdjustment
    };
    enum MMode
    {
        modeAC = 1,
        modeHF = 2,
        modeDC = 4,
        modeADJ = 8
    };
    enum SensorType
    {
        Direct = 0x100,
        Clamp = 0x200
    };
    const QString sVoltageChannelDescription = "Measuring channel 0..250V 50Hz/150kHz";
    const QString sCurrentChannelDescription = "Measuring channel 0..1000A 50Hz/150kHz";
    const QString sMeasuringModeDescription = "Measuring mode switch AC,HF,ADJ";
}

class Mt310s2SenseInterface : public cResource, public Mt310s2AdjFlash, public Mt310s2AdjXML
{
    Q_OBJECT
public:
    Mt310s2SenseInterface(cSCPI *scpiInterface, cI2CSettings *i2cSettings, cSenseSettings *senseSettings, Mt310s2SystemInfo *systemInfo);
    ~Mt310s2SenseInterface();
    virtual void initSCPIConnection(QString leadingNoMModedes) override;
    Mt310s2SenseChannel* getChannel(QString& name);
    QString getChannelSystemName(quint16 ctrlChannel);
    Mt310s2SenseRange* getRange(QString channelName, QString rangeName);
    virtual quint8 getAdjustmentStatus() override; // we return 0 if adj. otherwise  1 +2 +4
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    virtual void unregisterResource(RMConnection *rmConnection) override;
    virtual QString exportXMLString(int indent = 1) override;
    void m_ComputeSenseAdjData();
protected:
    void exportAdjData(QDataStream& stream) override;
    bool importAdjData(QDataStream& stream) override;
    bool importXMLDocument(QDomDocument* qdomdoc) override;
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    cSystemInfo* m_pSystemInfo;

    QList<Mt310s2SenseChannel*> m_ChannelList;
    QString m_sVersion;
    QString m_sMMode;
    QHash<QString,quint8> m_MModeHash;

    quint8 m_nVersionStatus;
    quint8 m_nSerialStatus;

    QString m_ReadVersion(QString& sInput);
    void m_ReadWriteMMode(cProtonetCommand* protoCmd);
    QString m_ReadMModeCatalog(QString& sInput);
    QString m_ReadSenseChannelCatalog(QString& sInput);
    QString m_ReadSenseGroupCatalog(QString& sInput);
    QString m_InitSenseAdjData(QString& sInput);
    QString m_ComputeSenseAdjData(QString& sInput);
    QString handleScpiReadAdjStatus(QString& sInput);
    Mt310s2JustRangeTripletOffsetGainPhase* CreateJustScpiInterfaceWithAtmelPermission();

    NotificationString notifierSenseMMode;
    NotificationString notifierSenseChannelCat;

    void setNotifierSenseMMode();
    void setNotifierSenseChannelCat();
    bool setSenseMode(QString sMode);
};

#endif // SENSEINTERFACE_H
