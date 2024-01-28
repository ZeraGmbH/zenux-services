#ifndef SENSEINTERFACECOMMON_H
#define SENSEINTERFACECOMMON_H

#include "resource.h"
#include "adjustmenteeprom.h"
#include "adjustmentxmlimportexporttemplate.h"
#include "i2csettings.h"
#include "systeminfo.h"
#include "sensechannelcommon.h"

namespace SenseSystem
{
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
}

class SenseInterfaceCommon : public cResource, public AdjustmentEeprom, public AdjustmentXmlImportExportTemplate, public AdjustmentStatusInterface
{
public:
    SenseInterfaceCommon(cSCPI *scpiInterface,
                         I2cSettings* i2cSettings,
                         SystemInfo *systemInfo,
                         AbstractFactoryI2cCtrlPtr ctrlFactory);
    virtual ~SenseInterfaceCommon();
    quint8 getAdjustmentStatus() override;
    SenseChannelCommon *getChannel(QString& name);
    QString getChannelSystemName(quint16 ctrlChannel);
    SenseRangeCommon* getRange(QString channelName, QString rangeName);
    void computeSenseAdjData();
    void registerResource(RMConnection *rmConnection, quint16 port) override;

protected:
    virtual void scpiReadWriteMMode(cProtonetCommand* protoCmd) = 0;
    virtual RangeAdjInterface* createJustScpiInterfaceWithAtmelPermission() = 0;
    QString scpiReadVersion(QString& scpi);
    QString m_ReadMModeCatalog(QString& scpi);
    QString m_ReadSenseChannelCatalog(QString& scpi);
    void setNotifierSenseMMode();
    void setNotifierSenseChannelCat();
    QStringList getSenseModesSortedById();

    static QString m_version;
    SystemInfo *m_systemInfo;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    QList<SenseChannelCommon*> m_channelList;
    QString m_currSenseMode;
    QHash<QString, int> m_availSenseModesHash;
    quint8 m_nSerialStatus;

    NotificationString m_notifierSenseMMode;
    NotificationString m_notifierSenseChannelCat;
};

#endif // SENSEINTERFACECOMMON_H
