#ifndef GENERATORINTERFACE_H
#define GENERATORINTERFACE_H

#include "abstractfactoryi2cctrl.h"
#include "generatorchannelinterface.h"
#include "sensesettings.h"
#include "sourcecontrolsettings.h"
#include <scpiserverconnection.h>
#include <QJsonObject>

class GeneratorInterface : public ScpiServerConnection
{
    Q_OBJECT
public:
    GeneratorInterface(std::shared_ptr<cSCPI> scpiInterface,
                       SourceControlSettings* settings,
                       cSenseSettingsPtr senseSettings,
                       const QList<GeneratorChannelInterface *> &channels,
                       AbstractFactoryI2cCtrlPtr ctrlFactory);
    void initSCPIConnection() override;
signals:
    void sigMeasRangeProbablyChanged(const QString &channelMName);

private:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
    QString scpiSourceModeOn(const QString &scpi);
    QString scpiSourceOn(const QString &scpi);
    static QJsonObject expandJsonCapabilities(const QJsonObject &capabilitiesRaw);

    QJsonObject m_generatorCapabilities;
    cSenseSettingsPtr m_senseSettings;
    QList<GeneratorChannelInterface *> m_channels;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    NotificationString m_sourceOnModesNotification;
};

#endif // GENERATORINTERFACE_H
