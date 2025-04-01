#ifndef MTXXXS2SENSECHANNEL_H
#define MTXXXS2SENSECHANNEL_H

#include "sensesettings.h"
#include <sensechannelcommon.h>

class MtXXXs2SenseChannel : public SenseChannelCommon
{
    Q_OBJECT
public:
    MtXXXs2SenseChannel(std::shared_ptr<cSCPI> scpiinterface,
                        QString unit,
                        SenseSystem::cChannelSettings* cSettings,
                        quint8 nr,
                        AbstractFactoryI2cCtrlPtr ctrlFactory);
    QString getAlias() override;
private:
    QString scpiReadWriteRange(QString& sInput) override;
    void setNotifierSenseChannelRange() override;
};

#endif // MTXXXS2SENSECHANNEL_H
