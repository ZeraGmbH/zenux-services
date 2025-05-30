#ifndef CLAMP_H
#define CLAMP_H

#include "senseinterfacecommon.h"
#include "pcbserver.h"
#include "adjustmentxmlimportexporttemplate.h"
#include "adjustmenteepromreadwrite.h"
#include <QList>
#include <QDateTime>
#include <QDomElement>

class cClamp: public AdjustmentXmlImportExportTemplate, public ScpiConnection
{
public:
    enum ClampTypes // APPEND NEW TYPES AT BOTTOM
    {
        undefined,
        CL120A,
        CL300A,
        CL1000A,
        EMOB32,
        EMOB200DC,
        EMOB80,
        RC3000,
        CL200ADC1000VDC, // LEM
        CL1000VDC,

        CL8ADC1000VDC,  // TESLA
        CL800ADC1000VDC,

        EMOB500DC,

        CL8ADC1400VDC,  // VDE
        CL800ADC1400VDC,
        CL1400VDC,

        CL1200ADC1400VDC, // LEM-3

        anzCL
    };
    cClamp();
    cClamp(PCBServer *server,
           I2cSettings *i2cSettings,
           SenseInterfaceCommon *senseInterface,
           QString channelName,
           quint8 ctrlChannel,
           I2cMuxerInterface::Ptr i2cMuxer,
           quint8 ctrlChannelSecondary,
           quint8 type = undefined);
    virtual ~cClamp();
    static QString getClampTypeName(quint8 type);
    virtual void initSCPIConnection(QString) override;
    QString getChannelName();
    QString getChannelNameSecondary();
    QString getSerial();
    virtual QString exportXMLString(int indent = 1) override;
    bool importXMLDocument(QDomDocument *qdomdoc, bool ignoreType);
    bool exportClampAdjData(QDateTime dateTimeWrite);
    bool importClampAdjData();

protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
    bool importXMLDocument(QDomDocument* qdomdoc) override;

private:
    bool isValidType();
    void initClamp(quint8 type);
    void addSense();
    void addSenseInterface();
    void addSystAdjInterface();
    void addSystAdjInterfaceChannel(QString channelName);
    void createLEM1000VRanges(const PermissionStructAdj &permissionsOffsetAllowedAlways);
    void createVDE1400VRanges();
    SenseRangeCommon* getRange(QString name);
    ClampTypes readClampType();
    void removeAllRanges();
    void exportRangeXml(QDomDocument &justqdom, QDomElement &typeTag, SenseRangeCommon *range);
    quint8 getAdjStatus();

    QString scpiReadWriteSerial(QString &scpi);
    QString scpiReadWriteVersion(QString &scpi);
    QString scpiReadWriteType(QString &scpi);
    QString scpiReadName(QString &scpi);
    QString scpiWriteFlash(QString &scpi);
    QString scpiReadFlash(QString &scpi);
    QString scpiResetFlash(QString &scpi);
    QString scpiReadChksum(QString &scpi);
    QString scpiWriteXML(QString &scpi);
    QString scpiReadXML(QString &scpi);
    QString scpiReadAdjStatus(QString &scpi);

    SenseInterfaceCommon *m_pSenseInterface = nullptr;

    QList<SenseRangeCommon*> m_RangeList;
    QString m_sChannelName;
    QList<SenseRangeCommon*> m_RangeListSecondary;
    QString m_sChannelNameSecondary;

    quint8 m_nCtrlChannel;
    quint8 m_nCtrlChannelSecondary;
    QString m_sSerial =  "1234567890"; // our default serial number
    QString m_sVersion = "unknown";
    quint8 m_nType = undefined;
    quint32 m_nFlags = 0; // for future purpose
    QDateTime m_AdjDateTime;
    AdjustmentEepromReadWrite m_adjReadWrite;
};

#endif // CLAMP_H
