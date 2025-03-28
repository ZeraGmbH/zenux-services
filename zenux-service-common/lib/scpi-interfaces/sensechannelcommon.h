#ifndef SENSECHANNELCOMMON_H
#define SENSECHANNELCOMMON_H

#include "abstractfactoryi2cctrl.h"
#include "senserangecommon.h"
#include "sensesettings.h"
#include <scpiconnection.h>

namespace SenseChannel
{
enum Commands
{
    cmdAlias,
    cmdType,
    cmdUnit,
    cmdDspChannel,
    cmdStatus,
    cmdStatusReset,
    cmdRange,
    cmdUrvalue,
    cmdRangeCat
};

}

class SenseChannelCommon : public ScpiConnection
{
public:
    SenseChannelCommon(std::shared_ptr<cSCPI> scpiinterface,
                       QString description,
                       QString unit,
                       SenseSystem::cChannelSettings* cSettings,
                       quint8 nr,
                       AbstractFactoryI2cCtrlPtr ctrlFactory);
    virtual ~SenseChannelCommon();

    void setRangeList(QList<SenseRangeCommon*>& list);
    const QList<SenseRangeCommon *> &getRangeList();
    SenseRangeCommon* getRange(QString& name);
    void addRangeList(QList<SenseRangeCommon*>& list);
    void removeRangeList(QList<SenseRangeCommon *> &list);

    quint8 getAdjustmentStatus80Mask();

    QString getName();
    QString getDescription();
    quint8 getCtrlChannel();
    // see COM5003 Com5003SenseInterface::registerSense()
    void setDescription(const QString& s);
    void setUnit(const QString &s);
    bool isAvail();

    void initJustData();
    void computeJustData();
    void initSCPIConnection(QString leadingNodes) override;

    void setMMode(int mode);

protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
    virtual QString getAlias() = 0;
    virtual void setNotifierSenseChannelRange() = 0;
    virtual QString scpiReadWriteRange(QString& sInput) = 0;

    QString m_ReadAlias(QString& sInput);
    QString m_ReadType(QString& sInput);
    QString m_ReadUnit(QString& sInput);
    QString m_ReadDspChannel(QString& sInput);
    QString m_ReadChannelStatus(QString& sInput);
    QString m_StatusReset(QString& sInput);

    QString m_ReadUrvalue(QString& sInput);
    QString m_ReadRangeCatalog(QString& sInput);

    void setNotifierSenseChannelRangeCat();

    const AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    const QString m_sName; // the channels name m0...
    const QString m_sAlias1; // the channel's alias name for example UL1
    const QString m_sAlias2; // the channel's alias name for example REF1 (COM5003 only)
    QString m_sDescription; // the channel's brief description
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
