#ifndef CLAMP_H
#define CLAMP_H

#include "adjflash.h"
#include "adjxml.h"
#include "scpi-interfaces/senseinterface.h"
#include <QList>
#include <QDateTime>
#include <QDomElement>

namespace clamp
{
    enum Commands
    {
        cmdSerial,
        cmdVersion,
        cmdType,
        cmdName,
        cmdFlashWrite,
        cmdFlashRead,
        cmdFlashReset,
        cmdChksum,
        cmdXMLWrite,
        cmdXMLRead,
        cmdStatAdjustment
    };
}

enum ClampTypes
{
    undefined,
    CL120A,
    CL300A,
    CL1000A,
    EMOB32,
    EMOB200DC,
    EMOB80,

    anzCL
};

class cClamp: public cAdjFlash, public cAdjXML, public cSCPIConnection
{
public:
    cClamp();
    cClamp(cMT310S2dServer *server, QString channelName, quint8 ctrlChannel, I2cMuxerInterface::Ptr i2cMuxer, quint8 ctrlChannelSecondary);
    virtual ~cClamp();
    virtual quint8 getAdjustmentStatus() override;
    virtual void initSCPIConnection(QString) override;
    QString getChannelName();
    QString getChannelNameSecondary();
    QString getSerial();
    virtual QString exportXMLString(int indent = 1) override;
    bool importXMLDocument(QDomDocument *qdomdoc, bool ignoreType);

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) override;

protected:
    virtual void exportAdjData(QDataStream& stream) override;
    virtual bool importAdjData(QDataStream& stream) override;
    virtual bool importXMLDocument(QDomDocument* qdomdoc) override;

private:
    void initClamp(quint8 type);
    void addSense();
    void addSenseInterface();
    void addSystAdjInterface();
    void addSystAdjInterfaceChannel(QString channelName);
    static QString getClampTypeName(quint8 type);
    cSenseRange* getRange(QString name);
    ClampTypes readClampType();
    void removeAllRanges();
    void exportRangeXml(QDomDocument &justqdom, QDomElement &typeTag, cSenseRange *range);

    QString handleScpiReadWriteSerial(QString &scpiCmdStr);
    QString handleScpiReadWriteVersion(QString &scpiCmdStr);
    QString handleScpiReadWriteType(QString &scpiCmdStr);
    QString handleScpiReadName(QString &scpiCmdStr);
    QString handleScpiWriteFlash(QString &scpiCmdStr);
    QString handleScpiReadFlash(QString &scpiCmdStr);
    QString handleScpiResetFlash(QString &scpiCmdStr);
    QString handleScpiReadChksum(QString &scpiCmdStr);
    QString handleScpiWriteXML(QString &scpiCmdStr);
    QString handleScpiReadXML(QString &scpiCmdStr);
    QString handleScpiReadAdjStatus(QString &scpiCmdStr);

    cSenseInterface *m_pSenseInterface = nullptr;

    QList<cSenseRange*> m_RangeList;
    QString m_sChannelName;
    QList<cSenseRange*> m_RangeListSecondary;
    QString m_sChannelNameSecondary;

    ushort m_i2cMuxAdress;
    quint8 m_nCtrlChannel;
    quint8 m_nCtrlChannelSecondary;
    QString m_sSerial;
    QString m_sVersion;
    quint8 m_nType; // 0 is undefined type
    quint32 m_nFlags; // for future purpose
    QDateTime m_AdjDateTime;
};

#endif // CLAMP_H
