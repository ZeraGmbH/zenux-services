#ifndef SENSECHANNEL_H
#define SENSECHANNEL_H

#include "sensesettings.h"
#include <QList>
#include <sensechannelcommon.h>

class Mt310s2SenseChannel : public SenseChannelCommon
{
    Q_OBJECT
public:
    Mt310s2SenseChannel(std::shared_ptr<cSCPI> scpiinterface,
                        QString description,
                        QString unit,
                        SenseSystem::cChannelSettings* cSettings,
                        quint8 nr,
                        AbstractFactoryI2cCtrlPtr ctrlFactory);
    void setMMode(int m) override;
    QString getAlias() override;
private:
    QString scpiReadWriteRange(QString& sInput) override;
    void setNotifierSenseChannelRange() override;
};

#endif // SENSECHANNEL_H
