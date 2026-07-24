#ifndef MOCKCLAMPPLUGGER_H
#define MOCKCLAMPPLUGGER_H

#include "i2csettings.h"
#include "sensesettings.h"

class MockClampPlugger
{
public:
    MockClampPlugger(const cSenseSettingsPtr &senseSettings, const I2cSettingsPtr &i2cSettings);
    static QByteArray getFreshEEpromData(int clampType);
    void initClampAdjMem(int clampType, QString channelAlias);
private:
    cSenseSettingsPtr m_senseSettings;
    I2cSettingsPtr m_i2cSettings;
};

#endif // MOCKCLAMPPLUGGER_H
