#ifndef GENERATORCHANNEL_H
#define GENERATORCHANNEL_H

#include "abstractfactoryi2cctrl.h"
#include "sensesettings.h"
#include <scpiconnection.h>
#include <memory>

class GeneratorChannel : public ScpiConnection
{
    Q_OBJECT
public:
    explicit GeneratorChannel(std::shared_ptr<cSCPI> scpiinterface,
                              const cSenseSettingsPtr senseSettings,
                              const SenseSystem::cChannelSettings* channelSettings,
                              AbstractFactoryI2cCtrlPtr ctrlFactory);
    void initSCPIConnection();
private:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
    QString scpiChangeRangeByAmplitude(const QString &scpi);
    QString scpiChangeRange(const QString &scpi);

    const cSenseSettingsPtr m_senseSettings;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    const QString m_mName;
    const QString m_alias;
};

#endif // GENERATORCHANNEL_H
