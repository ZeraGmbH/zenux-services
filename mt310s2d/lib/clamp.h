#ifndef CLAMP_H
#define CLAMP_H

#include "pcbserver.h"
#include "adjustmenteeprom.h"
#include "adjustmentxmlimportexporttemplate.h"
#include "mt310s2senseinterface.h"
#include <QList>
#include <QDateTime>
#include <QDomElement>

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
    CL200ADC1000VDC,
    CL1000VDC,

    CL8ADC1000VDC,
    CL800ADC1000VDC,

    EMOB500DC,

    anzCL
};

class cClamp: public AdjustmentEeprom, public AdjustmentXmlImportExportTemplate, public ScpiConnection
{
public:
    cClamp();
    cClamp(cPCBServer *server,
           I2cSettings *i2cSettings,
           Mt310s2SenseInterface *senseInterface,
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

protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
    void exportAdjData(QDataStream& stream, QDateTime dateTimeWrite) override;
    bool importAdjData(QDataStream& stream) override;
    bool importXMLDocument(QDomDocument* qdomdoc) override;

private:
    void initClamp(quint8 type);
    void addSense();
    void addSenseInterface();
    void addSystAdjInterface();
    void addSystAdjInterfaceChannel(QString channelName);
    void createLEM1000VRanges(const PermissionStructAdj &permissionsOffsetAllowedAlways, quint16 dcCommonMask);
    Mt310s2SenseRange* getRange(QString name);
    ClampTypes readClampType();
    void removeAllRanges();
    void exportRangeXml(QDomDocument &justqdom, QDomElement &typeTag, Mt310s2SenseRange *range);
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

    Mt310s2SenseInterface *m_pSenseInterface = nullptr;

    QList<Mt310s2SenseRange*> m_RangeList;
    QString m_sChannelName;
    QList<Mt310s2SenseRange*> m_RangeListSecondary;
    QString m_sChannelNameSecondary;

    quint8 m_nCtrlChannel;
    quint8 m_nCtrlChannelSecondary;
    QString m_sSerial =  "1234567890"; // our default serial number
    QString m_sVersion = "unknown";
    quint8 m_nType = undefined;
    quint32 m_nFlags = 0; // for future purpose
    QDateTime m_AdjDateTime;
};

#endif // CLAMP_H
