#include "settingsfordeviceserver.h"

SettingsForDeviceServer::SettingsForDeviceServer(ServerParams params) :
    m_params{params},
    m_i2cSettings(&m_xmlConfigReader),
    m_fpgaSettings(&m_xmlConfigReader)
{
    if(!m_xmlConfigReader.loadSchema(m_params.xsdFile))
        qFatal("Abort: Could not open xsd file '%s", qPrintable(m_params.xsdFile));

    connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
            &m_i2cSettings, &I2cSettings::configXMLInfo);
    connect(&m_xmlConfigReader, &Zera::XMLConfig::cReader::valueChanged,
            &m_fpgaSettings, &FPGASettings::configXMLInfo);


    if(!m_xmlConfigReader.loadXMLFile(m_params.xmlFile))
        qFatal("Abort: Could not open xml file '%s", qPrintable(m_params.xmlFile));
}

ServerParams SettingsForDeviceServer::getServerParams()
{
    return m_params;
}

I2cSettings *SettingsForDeviceServer::getI2cSettings()
{
    return &m_i2cSettings;
}

FPGASettings *SettingsForDeviceServer::getFpgaSettings()
{
    return &m_fpgaSettings;
}
