#ifndef SENSECHANNEL_H
#define SENSECHANNEL_H

#include "senserange.h"
#include "notificationstring.h"
#include "sensesettings.h"
#include "adjustmentstatusinterface.h"
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


class ScpiConnection;
class cSenseInterface;

class cSenseChannel : public ScpiConnection, public AdjustmentStatusInterface
{
    Q_OBJECT

public:
    cSenseChannel(cSCPI* scpiinterface, QString description, QString unit, SenseSystem::cChannelSettings* cSettings, quint8 nr);
    ~cSenseChannel();
    virtual void initSCPIConnection(QString leadingNodes) override;

    void setRangeList(QList<cSenseRange*>& list);
    QList<cSenseRange*>& getRangeList();
    void addRangeList(QList<cSenseRange*>& list);
    void removeRangeList(QList<cSenseRange*>& list);

    cSenseRange* getRange(QString& name);

    quint8 getAdjustmentStatus() override;

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
    QString m_sName; // the channels name m0...
    QString m_sAlias; // the channel's alias name for example UL1
    QString m_sDescription; // the channel's brief description
    QString m_sUnit; // the channel's measured unit
    quint8 m_nCtrlChannel; // where to control the channel
    quint8 m_nDspChannel; // where to find the channel's sampled data
    qint8 m_nOverloadBit;
    bool m_bAvail; // is this channel available ?
    QList<cSenseRange*> m_RangeList;
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