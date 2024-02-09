#ifndef SETTINGSCONTAINER_H
#define SETTINGSCONTAINER_H

#include "ethsettings.h"
#include "i2csettings.h"
#include "fpgasettings.h"
#include "samplingsettings.h"
#include <QObject>

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
    I2cSettings *getI2cSettings();
    FPGASettings *getFpgaSettings();
    SamplingSettings *getSamplingSettings();
private:
    ServerParams m_params;
    Zera::XMLConfig::cReader m_xmlConfigReader;
    EthSettings m_ethSettings;
    I2cSettings m_i2cSettings;
    FPGASettings m_fpgaSettings;
    SamplingSettings m_samplingSettings;
};

#endif // SETTINGSCONTAINER_H
