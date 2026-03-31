#ifndef GENERATORCHANNELINTERFACE_H
#define GENERATORCHANNELINTERFACE_H

#include "abstractfactoryi2cctrl.h"
#include "sensesettings.h"
#include <scpiconnection.h>
#include <memory>

class GeneratorChannelInterface : public ScpiConnection
{
    Q_OBJECT
public:
    explicit GeneratorChannelInterface(std::shared_ptr<cSCPI> scpiinterface,
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

#endif // GENERATORCHANNELINTERFACE_H
