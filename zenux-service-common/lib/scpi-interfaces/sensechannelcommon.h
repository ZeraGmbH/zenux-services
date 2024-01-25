#ifndef SENSECHANNELCOMMON_H
#define SENSECHANNELCOMMON_H

#include "abstractfactoryi2cctrl.h"
#include "senserangecommon.h"
#include "sensesettings.h"
#include <scpiconnection.h>

class SenseChannelCommon : public ScpiConnection
{
public:
    SenseChannelCommon(cSCPI* scpiinterface,
                       QString description,
                       QString unit,
                       SenseSystem::cChannelSettings* cSettings,
                       quint8 nr,
                       AbstractFactoryI2cCtrlPtr ctrlFactory);
    virtual ~SenseChannelCommon();
protected:
    virtual void setMMode(int m) = 0;
    virtual QString getAlias() = 0;
    virtual void setNotifierSenseChannelRange() = 0;
    virtual QString scpiReadWriteRange(QString& sInput) = 0;

    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    QString m_sName; // the channels name m0...
    QString m_sAlias1; // the channel's alias name for example UL1
    QString m_sAlias2; // the channel's alias name for example REF1 (COM5003 only)
    QString m_sDescription; // the channel's brief description
    QString m_sUnit; // the channel's measured unit
    quint8 m_nCtrlChannel; // where to control the channel
    quint8 m_nDspChannel; // where to find the channel's sampled data
    quint8 m_nOverloadBit;
    bool m_bAvail; // is this channel available ?
    QList<SenseRangeCommon*> m_RangeList;
    quint8 m_nMMode;
};

#endif // SENSECHANNELCOMMON_H
