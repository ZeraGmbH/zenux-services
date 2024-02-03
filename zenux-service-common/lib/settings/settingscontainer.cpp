#include "settingscontainer.h"

SettingsContainer::SettingsContainer(ServerParams params) :
    m_params{params},
    m_ethSettings(&m_xmlConfigReader),
    m_i2cSettings(&m_xmlConfigReader),
    m_fpgaSettings(&m_xmlConfigReader)
{
    if(!m_xmlConfigReader.loadSchema(m_params.xsdFile))
        qFatal("Abort: Could not open xsd file '%s", qPrintable(m_params.xsdFile));

    connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
            &m_ethSettings, &EthSettings::configXMLInfo);
    connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
            &m_i2cSettings, &I2cSettings::configXMLInfo);
    connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
            &m_fpgaSettings, &FPGASettings::configXMLInfo);


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
