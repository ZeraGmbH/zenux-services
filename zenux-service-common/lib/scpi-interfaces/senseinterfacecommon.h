#ifndef SENSEINTERFACECOMMON_H
#define SENSEINTERFACECOMMON_H

#include "resource.h"
#include "adjustmentxmlimportexporttemplate.h"
#include "i2csettings.h"
#include "systeminfo.h"
#include "sensechannelcommon.h"
#include "adjustmentdecoderinternal.h"
#include "adjustmenteepromreadwrite.h"

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

class SenseInterfaceCommon : public cResource, public AdjustmentXmlImportExportTemplate, public AdjustmentStatusInterface
{
public:
    SenseInterfaceCommon(cSCPI *scpiInterface,
                         I2cSettings* i2cSettings,
                         SystemInfo *systemInfo,
                         AbstractFactoryI2cCtrlPtr ctrlFactory);
    virtual ~SenseInterfaceCommon();
    quint8 getAdjustmentStatus() override;
    SenseChannelCommon *getChannel(QString& name);
    QString getChannelByCtrlChannelNo(quint16 ctrlChannel);
    SenseRangeCommon* getRange(QString channelName, QString rangeName);
    void computeSenseAdjData();
    void registerResource(RMConnection *rmConnection, quint16 port) override;
    bool exportAdjData(QDateTime dateTimeWrite);
    bool importAdjData();
    quint16 getAdjChecksum();

    virtual int rangeFlagsDevice() = 0;
    virtual int rangeFlagsIntern() = 0;
    virtual int rangeFlagsExtern() = 0;
    virtual int rangeFlagsExternDc() = 0; // maybe this can go - for now just compatibility
    virtual void initSCPIConnection(QString leadingNodes) override;
    QString exportXMLString(int indent = 1) override;

protected:
    virtual const char *getAdjExportedVersion() = 0; // stored from #define forces us using const char*
    virtual QString getPcbName() = 0;

    virtual RangeAdjInterface* createJustScpiInterfaceWithAtmelPermission() = 0;
    virtual void handleScpiReadWriteMMode(cProtonetCommand* protoCmd) = 0;
    virtual QString scpiReadSenseGroupCatalog(QString& scpi) = 0;

    virtual QString getXmlType() = 0;
    virtual bool isRangePartOfAdjXmlExport(SenseRangeCommon* range) = 0;
    bool importXMLDocument(QDomDocument* qdomdoc) override;

    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
    QString scpiReadVersion(QString& scpi);
    QString scpiReadMModeCatalog(QString& scpi);
    QString scpiReadSenseChannelCatalog(QString& scpi);
    QString scpiInitSenseAdjDataAllChannelRanges(QString& scpi);
    QString scpiComputeSenseAdjDataAllChannelRanges(QString& scpi);
    QString scpiReadAdjStatus(QString& scpi);

    void setNotifierSenseMMode();
    void setNotifierSenseChannelCat();
    QStringList getSenseModesSortedById();
    void decodeAdjustmentData();

    static QString m_version;
    SystemInfo *m_systemInfo;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    QList<SenseChannelCommon*> m_channelList;
    QString m_currSenseMode;
    QHash<QString, int> m_availSenseModesHash;
    quint8 m_nSerialStatus;

    NotificationString m_notifierSenseMMode;
    NotificationString m_notifierSenseChannelCat;

    AdjustmentEepromReadWrite m_adjReadWrite;
    AdjustmentDecoderInternal m_adjustmentDecoder;
};

#endif // SENSEINTERFACECOMMON_H
