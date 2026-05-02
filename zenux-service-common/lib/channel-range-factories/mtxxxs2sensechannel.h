#ifndef MTXXXS2SENSECHANNEL_H
#define MTXXXS2SENSECHANNEL_H

#include "sensesettings.h"
#include <sensechannelcommon.h>

static constexpr int rejectionScpiQueryDigitsMtXXXs2 = 8; // Changing this changes rejection => adjustment!!!

class MtXXXs2SenseChannel : public SenseChannelCommon
{
    Q_OBJECT
public:
    MtXXXs2SenseChannel(const std::shared_ptr<cSCPI> &scpiInterface,
                        const QString &unit,
                        SenseSystem::cChannelSettings* cSettings,
                        AbstractFactoryI2cCtrlPtr ctrlFactory);
    QString getAlias() override;
private:
    NotificationStatus scpiReadWriteRange(const ProtonetCommandPtr &protoCmd) override;
    void setNotifierSenseChannelRange() override;
};

#endif // MTXXXS2SENSECHANNEL_H
