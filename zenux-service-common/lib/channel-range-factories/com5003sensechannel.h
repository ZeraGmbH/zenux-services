#ifndef COM5003SENSECHANNEL_H
#define COM5003SENSECHANNEL_H

#include "sensesettings.h"
#include <sensechannelcommon.h>

static constexpr int rejectionScpiQueryDigitsCom5003 = 6; // Changing this changes rejection => adjustment!!!

class Com5003SenseChannel : public SenseChannelCommon
{
    Q_OBJECT
public:
    Com5003SenseChannel(std::shared_ptr<cSCPI> scpiinterface,
                        QString unit,
                        SenseSystem::cChannelSettings* cSettings,
                        quint8 nr,
                        AbstractFactoryI2cCtrlPtr ctrlFactory);
    QString getAlias() override;
private:
    QString scpiReadWriteRange(QString& sInput) override;
    void setNotifierSenseChannelRange() override;
};

#endif // COM5003SENSECHANNEL_H
