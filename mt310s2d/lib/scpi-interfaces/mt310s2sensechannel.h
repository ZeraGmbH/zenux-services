#ifndef SENSECHANNEL_H
#define SENSECHANNEL_H

#include "senserangecommon.h"
#include "notificationstring.h"
#include "sensesettings.h"
#include <QList>

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

class Mt310s2SenseChannel : public ScpiConnection
{
    Q_OBJECT

public:
    Mt310s2SenseChannel(cSCPI* scpiinterface,
                        QString description,
                        QString unit,
                        SenseSystem::cChannelSettings* cSettings,
                        quint8 nr,
                        AbstractFactoryI2cCtrlPtr ctrlFactory);
    ~Mt310s2SenseChannel();
    virtual void initSCPIConnection(QString leadingNodes) override;

    void setRangeList(QList<SenseRangeCommon*>& list);
    QList<SenseRangeCommon*>& getRangeList();
    SenseRangeCommon* getRange(QString& name);
    void addRangeList(QList<SenseRangeCommon*>& list);
    void removeRangeList(QList<SenseRangeCommon *> &list);

    quint8 getAdjustmentStatus80Mask();

    QString& getName();
    QString& getAlias();
    QString& getDescription();
    quint8 getCtrlChannel();
    void setDescription(const QString& s);
    void setUnit(QString& s);
    void setMMode(int m);
    bool isAvail();

    void initJustData();
    void computeJustData();

protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    QString m_sName; // the channels name m0...
    QString m_sAlias; // the channel's alias name for example UL1
    QString m_sDescription; // the channel's brief description
    QString m_sUnit; // the channel's measured unit
    quint8 m_nCtrlChannel; // where to control the channel
    quint8 m_nDspChannel; // where to find the channel's sampled data
    qint8 m_nOverloadBit;
    bool m_bAvail; // is this channel available ?
    QList<SenseRangeCommon*> m_RangeList;
    quint8 m_nMMode;

    QString m_ReadAlias(QString& sInput);
    QString m_ReadType(QString& sInput);
    QString m_ReadUnit(QString& sInput);
    QString m_ReadDspChannel(QString& sInput);
    QString m_ReadChannelStatus(QString& sInput);
    QString m_StatusReset(QString& sInput);

    QString m_ReadWriteRange(QString& sInput);
    QString m_ReadUrvalue(QString& sInput);
    QString m_ReadRangeCatalog(QString& sInput);

    NotificationString notifierSenseChannelRangeCat;
    NotificationString notifierSenseChannelRange;

    void setNotifierSenseChannelRangeCat();
    void setNotifierSenseChannelRange();
};

#endif // SENSECHANNEL_H
