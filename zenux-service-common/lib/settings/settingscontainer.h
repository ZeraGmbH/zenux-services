#ifndef SETTINGSCONTAINER_H
#define SETTINGSCONTAINER_H

#include "pcbserver.h"
#include "i2csettings.h"
#include "fpgasettings.h"
#include <QObject>

class SettingsContainer : public QObject
{
    Q_OBJECT
public:
    explicit SettingsContainer(ServerParams params);
    ServerParams getServerParams();
    I2cSettings *getI2cSettings();
    FPGASettings *getFpgaSettings();
private:
    ServerParams m_params;
    Zera::XMLConfig::cReader m_xmlConfigReader;
    I2cSettings m_i2cSettings;
    FPGASettings m_fpgaSettings;
};

#endif // SETTINGSCONTAINER_H
