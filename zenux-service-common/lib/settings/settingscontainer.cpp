#include "settingscontainer.h"

SettingsContainer::SettingsContainer(ServerParams params) :
    m_params{params},
    m_ethSettings(&m_xmlConfigReader),
    m_i2cSettings(&m_xmlConfigReader),
    m_fpgaSettings(&m_xmlConfigReader),
    m_samplingSettings(&m_xmlConfigReader)
{
    qInfo("Service settings: %s / %s", qPrintable(m_params.xsdFile), qPrintable(m_params.xmlFile));
    if(!m_xmlConfigReader.loadSchema(m_params.xsdFile))
        qFatal("Abort: Could not open xsd file '%s", qPrintable(m_params.xsdFile));

    connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
            &m_ethSettings, &EthSettings::configXMLInfo);
    connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
            &m_i2cSettings, &I2cSettings::configXMLInfo);
    connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
            &m_fpgaSettings, &FPGASettings::configXMLInfo);
    connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
            &m_samplingSettings, &SamplingSettings::configXMLInfo);

    if(!m_xmlConfigReader.loadXMLFile(m_params.xmlFile))
        qFatal("Abort: Could not open xml file '%s", qPrintable(m_params.xmlFile));
}

ServerParams SettingsContainer::getServerParams()
{
    return m_params;
}

EthSettings *SettingsContainer::getEthSettings()
{
    return &m_ethSettings;
}

I2cSettings *SettingsContainer::getI2cSettings()
{
    return &m_i2cSettings;
}

FPGASettings *SettingsContainer::getFpgaSettings()
{
    return &m_fpgaSettings;
}

SamplingSettings *SettingsContainer::getSamplingSettings()
{
    return &m_samplingSettings;
}

static const QMap<QString, SettingsContainer::TServiceConfig> supportedServiceConfigs {
   { "com5003d", {"com5003d.xsd", "com5003d.xml"}},
   { "mt310s2d", {"mtXXXs2d.xsd", "mt310s2d.xml"}},
   { "mt581s2d", {"mtXXXs2d.xsd", "mt581s2d.xml"}},
   { "sec1000d", {"sec1000d.xsd", "sec1000d.xml"}},
   { "zdsp1d",   {"zdsp1d.xsd",   "zdsp1d.xml"  }},
};

const SettingsContainer::TServiceConfig SettingsContainer::getServiceConfig(const QString &serviceName)
{
    if(supportedServiceConfigs.contains(serviceName))
        return supportedServiceConfigs[serviceName];
    if(supportedServiceConfigs.contains(serviceName + "d"))
        return supportedServiceConfigs[serviceName + "d"];
    qFatal("Unsupported service: %s", qPrintable(serviceName));
    return supportedServiceConfigs[serviceName];
}

