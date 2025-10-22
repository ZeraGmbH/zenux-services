#ifndef SENSECHANNELCOMMON_H
#define SENSECHANNELCOMMON_H

#include "abstractfactoryi2cctrl.h"
#include "delayedzeramcontrollerkeepalivecontainer.h"
#include "senserangecommon.h"
#include "sensesettings.h"
#include <scpiconnection.h>

class SenseChannelCommon : public ScpiConnection
{
    Q_OBJECT
public:
    SenseChannelCommon(std::shared_ptr<cSCPI> scpiinterface,
                       const QString &unit,
                       SenseSystem::cChannelSettings* cSettings,
                       quint8 nr,
                       AbstractFactoryI2cCtrlPtr ctrlFactory);
    virtual ~SenseChannelCommon();

    enum NotificationStatus {
        NotificationNow,
        NotificationDelayed
    };
    void setRangeList(const QList<SenseRangeCommon*>& list);
    const QList<SenseRangeCommon *> &getRangeList();
    SenseRangeCommon* getRange(const QString &name);
    void addRangeList(const QList<SenseRangeCommon*>& list);
    void removeRangeList(const QList<SenseRangeCommon *> &list);

    quint8 getAdjustmentStatus80Mask();

    QString getName();
    quint8 getCtrlChannel();
    void setUnit(const QString &s);
    bool isAvail();

    void initJustData();
    void computeJustData();
    void initSCPIConnection(QString leadingNodes) override;

    void setMMode(int mode);
    NotificationStatus setRangeCommon(SenseRangeCommon *range, ProtonetCommandPtr protoCmd);

protected:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
    virtual QString getAlias() = 0;
    virtual void setNotifierSenseChannelRange() = 0;
    virtual NotificationStatus scpiReadWriteRange(ProtonetCommandPtr protoCmd) = 0;

    QString scpiReadAlias(const QString& scpi);
    QString scpiReadType(const QString& scpi);
    QString scpiReadUnit(const QString& scpi);
    QString scpiReadDspChannel(const QString& scpi);
    QString scpiReadChannelStatus(const QString& scpi);
    QString scpiStatusReset(const QString& scpi);
    QString scpiReadUrvalue(const QString& scpi);
    QString scpiReadRangeCatalog(const QString& scpi);

    void setNotifierSenseChannelRangeCat();

    const AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    DelayedZeraMControllerKeepAliveContainer m_delayedCtrlIos;
    const QString m_sName; // the channels name m0...
    const QString m_sAlias1; // the channel's alias name for example UL1
    const QString m_sAlias2; // the channel's alias name for example REF1 (COM5003 only)
    QString m_sUnit; // the channel's measured unit
    const quint8 m_nCtrlChannel; // where to control the channel
    const quint8 m_nDspChannel; // where to find the channel's sampled data
    const quint8 m_nOverloadBit;
    const bool m_bAvail; // is this channel available ?
    QList<SenseRangeCommon*> m_RangeList;
    quint8 m_nMMode;

    NotificationString notifierSenseChannelRangeCat;
    NotificationString notifierSenseChannelRange;
};

#endif // SENSECHANNELCOMMON_H
