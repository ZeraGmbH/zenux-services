#ifndef SENSECHANNELCOMMON_H
#define SENSECHANNELCOMMON_H

#include "abstractfactoryi2cctrl.h"
#include "asyncqueuedmcontrollerio.h"
#include "senserangecommon.h"
#include "sensesettings.h"
#include <scpiconnection.h>

class SenseChannelCommon : public ScpiConnection
{
    Q_OBJECT
public:
    SenseChannelCommon(const std::shared_ptr<cSCPI> &scpiInterface,
                       const QString &unit,
                       SenseSystem::cChannelSettings* cSettings,
                       const AbstractFactoryI2cCtrlPtr &ctrlFactory);
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

    bool initJustData();
    bool computeJustData();
    void initSCPIConnection(const QString &leadingNodes);

    void setMMode(int mode);

    NotificationStatus setRangeCommon(SenseRangeCommon *range, const ProtonetCommandPtr &protoCmd);
    virtual void refreshNotifierForActualRange() = 0;

protected:
    void executeProtoScpi(int cmdCode, const ProtonetCommandPtr &protoCmd) override;
    virtual QString getAlias() = 0;
    virtual NotificationStatus scpiReadWriteRange(const ProtonetCommandPtr &protoCmd) = 0;

    QString scpiReadAlias(const QString& scpi);
    QString scpiReadType(const QString& scpi);
    QString scpiReadUnit(const QString& scpi);
    QString scpiReadDspChannel(const QString& scpi);
    QString scpiReadUrvalue(const QString& scpi);
    QString scpiReadRangeCatalog(const QString& scpi);

    QString scpiReadChannelStatus(const QString& scpi);
    QString scpiStatusReset(const QString& scpi);
    QString scpiReadHardOverload(const QString& scpi);

    void setNotifierSenseChannelRangeCat();

    const AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    AsyncQueuedMControllerIo m_delayedCtrlIos;
    const SenseSystem::cChannelSettings m_settings;
    QList<SenseRangeCommon*> m_RangeList;
    QString m_sUnit;
    quint8 m_nMMode;

    NotificationString notifierSenseChannelRangeCat;
    NotificationString notifierSenseChannelRange;
};

#endif // SENSECHANNELCOMMON_H
