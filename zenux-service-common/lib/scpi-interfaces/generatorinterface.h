#ifndef GENERATORINTERFACE_H
#define GENERATORINTERFACE_H

#include "abstractfactoryi2cctrl.h"
#include "sensesettings.h"
#include <scpiserverconnection.h>

class GeneratorInterface : public ScpiServerConnection
{
    Q_OBJECT
public:
    GeneratorInterface(std::shared_ptr<cSCPI> scpiInterface,
                       cSenseSettingsPtr senseSettings,
                       AbstractFactoryI2cCtrlPtr ctrlFactory);
    void initSCPIConnection() override;

private:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
    QString scpiSourceModeOn(const QString &scpi);
    QString scpiSourceOn(const QString &scpi);
    QString scpiChangeRangeByAmplitude(const QString &scpi);
    QString scpiChangeRange(const QString &scpi);

    cSenseSettingsPtr m_senseSettings;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    NotificationString m_sourceOnModesNotification;
};

#endif // GENERATORINTERFACE_H
