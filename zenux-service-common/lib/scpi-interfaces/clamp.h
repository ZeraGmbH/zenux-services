#ifndef CLAMP_H
#define CLAMP_H

#include "senseinterfacecommon.h"
#include "pcbserver.h"
#include "adjustmentxmlimportexporttemplate.h"
#include "adjustmenteepromreadwrite.h"
#include <QList>
#include <QDateTime>
#include <QDomElement>

static const QString ClampDefaultSerNo = QStringLiteral("1234567890");
static const QString ClampDefaultVersion = QStringLiteral("unknown");

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

        EMOB750DC,

        anzCL
    };
    cClamp(PCBServer *server,
           SenseInterfaceCommon *senseInterface,
           const SenseSystem::cChannelSettings *chSettings,
           EepromI2cDeviceInterfacePtr adjMemory,
           quint8 ctrlChannelSecondary);
    virtual ~cClamp();
    static bool isValidClampType(quint8 type);
    static QString getClampTypeName(quint8 type);
    virtual void initSCPIConnection(QString) override;
    virtual QString exportXMLString(int indent = 1) override;

    const SenseSystem::cChannelSettings *getChannelSettings() const;
    QString getChannelName() const;
    QString getChannelNameSecondary() const;
    QString getSerial();
    bool importXMLDocument(QDomDocument *qdomdoc, bool ignoreType);
    static bool importXmlForSerialNo(QDomDocument *qdomdoc, QString &serialNo);
    bool exportClampAdjData(const QDateTime &dateTimeWrite);
    bool importClampAdjData();

protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
    bool importXMLDocument(QDomDocument* qdomdoc) override;

private:
    static SenseRangeCommon* getRangeStatic(const QString &name,
                                            const QList<SenseRangeCommon *> &rangeList,
                                            const QList<SenseRangeCommon *> &rangeListSecondary);
    static bool importXMLDocumentStatic(QDomDocument *qdomdoc, bool ignoreType,
                                        quint8 clamptype, QString &serialNo, QString &version,
                                        const QList<SenseRangeCommon *> &rangeList, const QList<SenseRangeCommon *> &rangeListSecondary);
    bool isValidType();
    void initClamp(quint8 type);
    void addSense();
    void addSenseInterface();
    void addSystAdjInterface();
    void addSystAdjInterfaceChannel(const QString &channelName);
    void createLEM1000VRanges(const PermissionStructAdj &permissions);
    void createVDE1400VRanges(const PermissionStructAdj &permissions);
    SenseRangeCommon* getRange(const QString &name);
    ClampTypes readClampType();
    void removeAllRanges();
    void exportRangeXml(QDomDocument &justqdom, QDomElement &typeTag, SenseRangeCommon *range);
    quint8 getAdjStatus();

    QString scpiReadWriteSerial(const QString &scpi);
    QString scpiReadWriteVersion(const QString &scpi);
    QString scpiReadWriteType(const QString &scpi);
    QString scpiReadName(const QString &scpi);
    QString scpiWriteFlash(const QString &scpi);
    QString scpiReadFlash(const QString &scpi);
    QString scpiResetFlash(const QString &scpi);
    QString scpiReadChksum(const QString &scpi);
    QString scpiWriteXML(const QString &scpi);
    QString scpiReadXML(const QString &scpi);
    QString scpiReadAdjStatus(const QString &scpi);

    SenseInterfaceCommon *m_pSenseInterface = nullptr;
    const SenseSystem::cChannelSettings *m_chSettings;
    QList<SenseRangeCommon*> m_RangeList;
    QList<SenseRangeCommon*> m_RangeListSecondary;
    QString m_sChannelNameSecondary;

    quint8 m_nCtrlChannelSecondary;
    QString m_sSerial =  ClampDefaultSerNo;
    QString m_sVersion = ClampDefaultVersion;
    quint8 m_nType = undefined;
    quint32 m_nFlags = 0; // for future purpose
    QDateTime m_AdjDateTime;
    AdjustmentEepromReadWrite m_adjReadWrite;
};

#endif // CLAMP_H
