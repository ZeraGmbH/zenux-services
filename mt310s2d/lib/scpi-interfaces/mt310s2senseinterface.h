#ifndef SENSEINTERFACE_H
#define SENSEINTERFACE_H

#include "i2csettings.h"
#include "mt310s2adjflash.h"
#include "adjustmentxmlimportexporttemplate.h"
#include "rangeadjinterface.h"
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
        // This is protection of intellectual property by obscurity at it's best.
        // To shed some light see:
        //
        // * test_regression_sense_interface_mt310s2: all the tests with mode setting
        // * Mt310s2SenseInterface::Mt310s2SenseInterface on setup of m_MModeHash
        // * Mt310s2SenseRange::setMMode for sense range (de)activation
        // * Mt310s2SenseInterface::setSenseMode to see some nasty bitmanipulation
        //   before asking atmel to change mode.
        //
        // To make things even worse: COM5003 has other ideas on modes (seach for
        // 'enum MMode' and 'Com5003SenseInterface::registerSense()') and howto handle
        // mode change...

        // ranges with this are active on mmode 'AC' (standard mode)
        modeAC = 1,

        // ranges with this are active on mmode 'HF' At the time of writing no range supporst this!!!
        // Maybe more ingenious concepts never finished and causing questions now?
        modeHF = 2,

        // there is no mmode for ranges with this - we could remove that from DC clamps or implement some
        // superclever code using this flag???
        modeDC = 4,

        // ranges with this are active on mmode 'ADJ'. This is used to enable internal clamp (voltage-)ranges
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

class Mt310s2SenseInterface : public cResource, public Mt310s2AdjFlash, public AdjustmentXmlImportExportTemplate
{
    Q_OBJECT
public:
    Mt310s2SenseInterface(cSCPI *scpiInterface,
                          cI2CSettings *i2cSettings,
                          cSenseSettings *senseSettings,
                          cSystemInfo *systemInfo,
                          AtmelPermissionTemplate *permissionQueryHandler);
    ~Mt310s2SenseInterface();
    virtual void initSCPIConnection(QString leadingNoMModedes) override;
    Mt310s2SenseChannel* getChannel(QString& name);
    QString getChannelSystemName(quint16 ctrlChannel);
    Mt310s2SenseRange* getRange(QString channelName, QString rangeName);
    virtual quint8 getAdjustmentStatus() override; // we return 0 if adj. otherwise  1 +2 +4
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
    virtual QString exportXMLString(int indent = 1) override;
    void computeSenseAdjData();
protected:
    void exportAdjData(QDataStream& stream, QDateTime dateTimeWrite) override;
    bool importAdjData(QDataStream& stream) override;
    bool importXMLDocument(QDomDocument* qdomdoc) override;
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString scpiReadVersion(QString& scpi);
    void scpiReadWriteMMode(cProtonetCommand* protoCmd);
    QString m_ReadMModeCatalog(QString& scpi);
    QString m_ReadSenseChannelCatalog(QString& sInput);
    QString m_ReadSenseGroupCatalog(QString& sInput);
    QString m_InitSenseAdjData(QString& sInput);
    QString m_ComputeSenseAdjData(QString& sInput);
    QString scpiReadAdjStatus(QString& sInput);
    RangeAdjInterface* createJustScpiInterfaceWithAtmelPermission();
    void setNotifierSenseMMode();
    void setNotifierSenseChannelCat();
    bool setSenseMode(QString sMode);

    cSystemInfo* m_pSystemInfo;
    AtmelPermissionTemplate *m_permissionQueryHandler;

    QList<Mt310s2SenseChannel*> m_ChannelList;
    QString m_sVersion;
    QString m_sMMode;
    QHash<QString,quint8> m_MModeHash;

    quint8 m_nSerialStatus;


    NotificationString notifierSenseMMode;
    NotificationString notifierSenseChannelCat;

};

#endif // SENSEINTERFACE_H
