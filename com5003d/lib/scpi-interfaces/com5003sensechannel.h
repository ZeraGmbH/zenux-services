#ifndef SENSECHANNEL_H
#define SENSECHANNEL_H

#include "com5003senserange.h"
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

enum MMode
{
    modeAC,
    modeREF,
};

}


class ScpiConnection;


class Com5003SenseChannel : public ScpiConnection
{
    Q_OBJECT

public:
    Com5003SenseChannel(cSCPI* scpiinterface, QString description, QString unit, SenseSystem::cChannelSettings* cSettings, quint8 nr);
    ~Com5003SenseChannel();
    virtual void initSCPIConnection(QString leadingNodes) override;

    void setRangeList(QList<Com5003SenseRange*>& list);
    QList<Com5003SenseRange*>& getRangeList();
    Com5003SenseRange* getRange(QString& name);

    quint8 getAdjustmentStatus80Mask();

    QString& getName();
    QString& getAlias();
    QString& getDescription();
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
    QString m_sAlias[2]; // the channel's alias name for example UL1 alternativ REF1
    QString m_sDescription; // the channel's brief description
    QString m_sUnit; // the channel's measured unit
    quint8 m_nCtrlChannel; // where to control the channel
    quint8 m_nDspChannel; // where to find the channel's sampled data
    quint8 m_nOverloadBit;
    bool m_bAvail; // is this channel available ?
    QList<Com5003SenseRange*> m_RangeList;
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
