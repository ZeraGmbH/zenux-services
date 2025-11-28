#ifndef SETTINGSCONTAINER_H
#define SETTINGSCONTAINER_H

#include "abstractchannelrangefactory.h"
#include "ethsettings.h"
#include "i2csettings.h"
#include "sensesettings.h"
#include "fpgasettings.h"
#include "samplingsettings.h"
#include <QObject>
#include <memory>

struct ServerParams
{
    ServerParams(int channelCount,
                 const QString &nameReported,
                 const QString &versionReported,
                 const QString &xsdFile,
                 const QString &xmlFile);
    int getChannelCount() const;
    QString getNameReported() const;
    QString getVersionReported() const;
    QString getXsdFile() const;
    QString getXmlFile() const;
private:
    int m_channelCount;
    QString m_nameReported;
    QString m_versionReported;
    QString m_xsdFile;
    QString m_xmlFile;
};

class SettingsContainer : public QObject
{
    Q_OBJECT
public:
    explicit SettingsContainer(ServerParams params);
    ServerParams getServerParams();

    EthSettingsPtr getEthSettings() const;
    I2cSettingsPtr getI2cSettings() const;
    cSenseSettingsPtr getSenseSettings() const;
    FPGASettingsPtr getFpgaSettings() const;
    SamplingSettingsPtr getSamplingSettings() const;
    struct TServiceConfig
    {
        QString xsdFileName;
        QString xmlFileName;
    };
    static const TServiceConfig getServiceConfig(const QString &serviceOrDeviceName);
    static AbstractChannelRangeFactoryPtr createChannelRangeFactory(const QString &serviceOrDeviceName);
private:
    ServerParams m_params;
    Zera::XMLConfig::cReader m_xmlConfigReader;
    EthSettingsPtr m_ethSettings;
    I2cSettingsPtr m_i2cSettings;
    cSenseSettingsPtr m_senseSettings;
    FPGASettingsPtr m_fpgaSettings;
    SamplingSettingsPtr m_samplingSettings;
};

typedef std::unique_ptr<SettingsContainer> SettingsContainerPtr;

#endif // SETTINGSCONTAINER_H
