#ifndef SETTINGSCONTAINER_H
#define SETTINGSCONTAINER_H

#include "abstractchannelrangefactory.h"
#include "ethsettings.h"
#include "i2csettings.h"
#include "fpgasettings.h"
#include "samplingsettings.h"
#include <QObject>
#include <memory>

struct ServerParams
{
    QString name;
    QString version;
    QString xsdFile;
    QString xmlFile;
};

class SettingsContainer : public QObject
{
    Q_OBJECT
public:
    explicit SettingsContainer(ServerParams params);
    ServerParams getServerParams();
    EthSettings *getEthSettings();
    const I2cSettings *getI2cSettings() const;
    FPGASettings *getFpgaSettings();
    SamplingSettings *getSamplingSettings();
    struct TServiceConfig
    {
        QString xsdFileName;
        QString xmlFileName;
    };
    static const TServiceConfig getServiceConfig(const QString &serviceNameForAlternateDevice);
    static AbstractChannelRangeFactoryPtr createChannelRangeFactory(const QString &serviceNameForAlternateDevice);
private:
    ServerParams m_params;
    Zera::XMLConfig::cReader m_xmlConfigReader;
    EthSettings m_ethSettings;
    I2cSettings m_i2cSettings;
    FPGASettings m_fpgaSettings;
    SamplingSettings m_samplingSettings;
};

typedef std::unique_ptr<SettingsContainer> SettingsContainerPtr;

#endif // SETTINGSCONTAINER_H
