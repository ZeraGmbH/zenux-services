#ifndef SENSEINTERFACECOMMON_H
#define SENSEINTERFACECOMMON_H

#include "abstractadjstatus.h"
#include "abstractchannelrangefactory.h"
#include "resource.h"
#include "adjustmentxmlimportexporttemplate.h"
#include "i2csettings.h"
#include "systeminfo.h"
#include "sensechannelcommon.h"
#include "adjustmenteepromreadwrite.h"

enum MMode
{
    // This is protection of intellectual property by obscurity at it's best.
    // To shed some light see:
    //
    // * test_regression_sense_interface_mt310s2: all the tests with mode setting
    // * Mt310s2SenseInterface::Mt310s2SenseInterface on setup of m_availSenseModesHash
    // * SenseRangeCommon::setMMode for sense range (de)activation
    // * Mt310s2SenseInterface::setSenseMode to see some nasty bitmanipulation
    //   before asking atmel to change mode.
    //
    // To make things even worse: COM5003 has other ideas on modes (seach for
    // 'enum MMode' and 'Com5003SenseInterface::registerSense()') and howto handle
    // mode change...

    // ranges with this are active on mmode 'AC' (standard mode)
    modeAC = 1<<0,

    // ranges with this are active on mmode 'HF' At the time of writing no range supporst this!!!
    // Maybe more ingenious concepts never finished and causing questions now?
    modeHF = 1<<1,

    // there is no mmode for ranges with this - we could remove that from DC clamps or implement some
    // superclever code using this flag???
    modeDC = 1<<2,

    // ranges with this are active on mmode 'ADJ'. This is used to enable internal clamp (voltage-)ranges
    modeADJ = 1<<3,

    // COM5003 only reference mode / range R0V/R10V hack
    modeREF = 1<<4,
};

enum SensorType {
    Direct = 1<<8,
    Clamp = 1<<9
};

class SenseInterfaceCommon : public cResource, public AdjustmentXmlImportExportTemplate, public AbstractAdjStatus
{
public:
    SenseInterfaceCommon(std::shared_ptr<cSCPI> scpiInterface,
                         I2cSettings* i2cSettings,
                         SystemInfo *systemInfo,
                         cSenseSettings* senseSettings,
                         AbstractChannelRangeFactoryPtr rangeFactory,
                         AbstractFactoryI2cCtrlPtr ctrlFactory,
                         QHash<QString, int> availSenseModesHash);
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

    int rangeFlagsExtern();
    int rangeFlagsExternDc(); // maybe this can go - for now just compatibility
    virtual void initSCPIConnection(QString leadingNodes) override;
    QString exportXMLString(int indent = 1) override;

protected:
    virtual const char *getAdjExportedVersion() = 0; // stored from #define forces us using const char*
    virtual QString getPcbName() = 0;

    virtual QString scpiReadSenseGroupCatalog(QString& scpi) = 0;
    virtual bool setSenseMode(QString modeStr) = 0;

    virtual QString getXmlType() = 0;
    virtual bool isRangePartOfAdjXmlExport(SenseRangeCommon* range) = 0;
    bool importXMLDocument(QDomDocument* qdomdoc) override;

    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;

    void setNotifierSenseMMode();
    void setNotifierSenseChannelCat();
    void injectAdjToChannelRanges();

    static QString m_version;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    QList<SenseChannelCommon*> m_channelList;
    QString m_currSenseMode;
    const QHash<QString, int> m_availSenseModesHash;
    AdjDataPtr m_adjData;

private:
    void handleScpiReadWriteMMode(cProtonetCommand* protoCmd);
    QString scpiReadVersion(QString& scpi);
    QString scpiReadMModeCatalog(QString& scpi);
    QString scpiReadSenseChannelCatalog(QString& scpi);
    QString scpiInitSenseAdjDataAllChannelRanges(QString& scpi);
    QString scpiComputeSenseAdjDataAllChannelRanges(QString& scpi);
    QString scpiReadAdjStatus(QString& scpi);
    QStringList getSenseModesSortedById();

    AdjustmentEepromReadWrite m_adjReadWrite;
    SystemInfo *m_systemInfo;
    quint8 m_nSerialStatus;
    NotificationString m_notifierSenseMMode;
    NotificationString m_notifierSenseChannelCat;
};

#endif // SENSEINTERFACECOMMON_H
