#ifndef SENSECHANNEL_H
#define SENSECHANNEL_H

#include "senserangecommon.h"
#include "notificationstring.h"
#include "sensesettings.h"
#include <QList>
#include <sensechannelcommon.h>

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

class Com5003SenseChannel : public SenseChannelCommon
{
    Q_OBJECT

public:
    Com5003SenseChannel(cSCPI* scpiinterface,
                        QString description,
                        QString unit,
                        SenseSystem::cChannelSettings* cSettings,
                        quint8 nr,
                        AbstractFactoryI2cCtrlPtr ctrlFactory);
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
