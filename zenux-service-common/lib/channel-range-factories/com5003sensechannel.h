#ifndef COM5003SENSECHANNEL_H
#define COM5003SENSECHANNEL_H

#include "sensesettings.h"
#include <sensechannelcommon.h>

static constexpr int rejectionScpiQueryDigitsCom5003 = 6; // Changing this changes rejection => adjustment!!!

class Com5003SenseChannel : public SenseChannelCommon
{
    Q_OBJECT
public:
    Com5003SenseChannel(const std::shared_ptr<cSCPI> &scpiInterface,
                        const QString &unit,
                        SenseSystem::cChannelSettings* cSettings,
                        AbstractFactoryI2cCtrlPtr ctrlFactory);
    QString getAlias() override;
private:
    NotificationStatus scpiReadWriteRange(const ProtonetCommandPtr &protoCmd) override;
    void setNotifierSenseChannelRange() override;
};

#endif // COM5003SENSECHANNEL_H
