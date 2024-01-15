#ifndef SETTINGSFORDEVICESERVER_H
#define SETTINGSFORDEVICESERVER_H

#include "pcbserver.h"
#include "i2csettings.h"
#include "fpgasettings.h"
#include <QObject>

class SettingsForDeviceServer : public QObject
{
    Q_OBJECT
public:
    explicit SettingsForDeviceServer(ServerParams params);
    ServerParams getServerParams();
    cI2CSettings *getI2cSettings();
    FPGASettings *getFpgaSettings();
private:
    ServerParams m_params;
    Zera::XMLConfig::cReader m_xmlConfigReader;
    cI2CSettings m_i2cSettings;
    FPGASettings m_fpgaSettings;
};

#endif // SETTINGSFORDEVICESERVER_H
