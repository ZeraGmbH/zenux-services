#include "settingscontainer.h"
#include "com5003channelrangefactory.h"
#include "mt310s2channelrangefactory.h"
#include "mt581s2channelrangefactory.h"
#include <functional>

ServerParams::ServerParams(int channelCount,
                           const QString &nameReported,
                           const QString &versionReported,
                           const QString &xsdFile,
                           const QString &xmlFile) :
    m_channelCount(channelCount),
    m_nameReported(nameReported),
    m_versionReported(versionReported),
    m_xsdFile(xsdFile),
    m_xmlFile(xmlFile)
{
}

int ServerParams::getChannelCount() const
{
    return m_channelCount;
}

QString ServerParams::getNameReported() const
{
    return m_nameReported;
}

QString ServerParams::getVersionReported() const
{
    return m_versionReported;
}

QString ServerParams::getXsdFile() const
{
    return m_xsdFile;
}

QString ServerParams::getXmlFile() const
{
    return m_xmlFile;
}

SettingsContainer::SettingsContainer(ServerParams params) :
    m_params{params},
    m_ethSettings(std::make_shared<EthSettings>(&m_xmlConfigReader)),
    m_i2cSettings(std::make_shared<I2cSettings>(&m_xmlConfigReader)),
    m_senseSettings(std::make_shared<cSenseSettings>(&m_xmlConfigReader, params.getChannelCount())),
    m_fpgaSettings(std::make_shared<FPGASettings>(&m_xmlConfigReader)),
    m_samplingSettings(std::make_shared<SamplingSettings>(&m_xmlConfigReader))
{
    connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
            m_ethSettings.get(), &EthSettings::configXMLInfo);
    connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
            m_i2cSettings.get(), &I2cSettings::configXMLInfo);
    connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
            m_senseSettings.get(), &cSenseSettings::configXMLInfo);
    connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
            m_fpgaSettings.get(), &FPGASettings::configXMLInfo);
    connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
            m_samplingSettings.get(), &SamplingSettings::configXMLInfo);

    if(!m_xmlConfigReader.loadXMLFile(m_params.getXmlFile()))
        qFatal("Abort: Could not open xml file '%s", qPrintable(m_params.getXmlFile()));
}

ServerParams SettingsContainer::getServerParams()
{
    return m_params;
}

EthSettingsPtr SettingsContainer::getEthSettings() const
{
    return m_ethSettings;
}

I2cSettingsPtr SettingsContainer::getI2cSettings() const
{
    return m_i2cSettings;
}

cSenseSettingsPtr SettingsContainer::getSenseSettings() const
{
    return m_senseSettings;
}

FPGASettingsPtr SettingsContainer::getFpgaSettings() const
{
    return m_fpgaSettings;
}

SamplingSettingsPtr SettingsContainer::getSamplingSettings() const
{
    return m_samplingSettings;
}

AbstractChannelRangeFactoryPtr SettingsContainer::createChannelRangeFactory(const QString &serviceOrDeviceName)
{
    static const QMap<QString, std::function<AbstractChannelRangeFactoryPtr()>> supportedChannelRangeFactories {
        { "com5003d", []() { return std::make_shared<COM5003ChannelRangeFactory>(); }},
        { "mt310s2d", []() { return std::make_shared<MT310s2ChannelRangeFactory>(); }},
        { "mt581s2d", []() { return std::make_shared<MT581s2ChannelRangeFactory>(); }}
    };
    if(supportedChannelRangeFactories.contains(serviceOrDeviceName))
        return supportedChannelRangeFactories[serviceOrDeviceName]();
    if(supportedChannelRangeFactories.contains(serviceOrDeviceName + "d"))
        return supportedChannelRangeFactories[serviceOrDeviceName + "d"]();
    qFatal("Unsupported service: %s", qPrintable(serviceOrDeviceName));
    return nullptr;

}

const SettingsContainer::TServiceConfig SettingsContainer::getServiceConfig(const QString &serviceOrDeviceName)
{
    static const QMap<QString, SettingsContainer::TServiceConfig> supportedServiceConfigs {
        { "com5003d", {"com5003d.xsd", "com5003d.xml"}},
        { "mt310s2d", {"mtXXXs2d.xsd", "mt310s2d.xml"}},
        { "mt581s2d", {"mtXXXs2d.xsd", "mt581s2d.xml"}},
        { "sec1000d", {"sec1000d.xsd", "sec1000d.xml"}},
        { "zdsp1d",   {"zdsp1d.xsd",   "zdsp1d.xml"  }},
    };
    if(supportedServiceConfigs.contains(serviceOrDeviceName))
        return supportedServiceConfigs[serviceOrDeviceName];
    if(supportedServiceConfigs.contains(serviceOrDeviceName + "d"))
        return supportedServiceConfigs[serviceOrDeviceName + "d"];
    qFatal("Unsupported service: %s", qPrintable(serviceOrDeviceName));
    return supportedServiceConfigs[serviceOrDeviceName];
}
