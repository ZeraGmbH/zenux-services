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

}

class Mt310s2SenseChannel : public SenseChannelCommon
{
    Q_OBJECT

public:
    Mt310s2SenseChannel(cSCPI* scpiinterface,
                        QString description,
                        QString unit,
                        SenseSystem::cChannelSettings* cSettings,
                        quint8 nr,
                        AbstractFactoryI2cCtrlPtr ctrlFactory);
    void setMMode(int m) override;
    QString getAlias() override;
    void initSCPIConnection(QString leadingNodes) override;

    void setRangeList(QList<SenseRangeCommon*>& list);
    QList<SenseRangeCommon*>& getRangeList();
    SenseRangeCommon* getRange(QString& name);
    void addRangeList(QList<SenseRangeCommon*>& list);
    void removeRangeList(QList<SenseRangeCommon *> &list);

    quint8 getAdjustmentStatus80Mask();

    QString getName();
    QString getDescription();
    quint8 getCtrlChannel();
    void setDescription(const QString& s);
    void setUnit(QString& s);
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

    QString scpiReadWriteRange(QString& sInput) override;
    QString m_ReadUrvalue(QString& sInput);
    QString m_ReadRangeCatalog(QString& sInput);

    void setNotifierSenseChannelRangeCat();
    void setNotifierSenseChannelRange() override;
};

#endif // SENSECHANNEL_H
