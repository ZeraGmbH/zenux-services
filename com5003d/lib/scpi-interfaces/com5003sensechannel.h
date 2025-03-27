#ifndef COM5003SENSECHANNEL_H
#define COM5003SENSECHANNEL_H

#include "sensesettings.h"
#include <QList>
#include <sensechannelcommon.h>

class Com5003SenseChannel : public SenseChannelCommon
{
    Q_OBJECT
public:
    Com5003SenseChannel(std::shared_ptr<cSCPI> scpiinterface,
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

#endif // COM5003SENSECHANNEL_H
