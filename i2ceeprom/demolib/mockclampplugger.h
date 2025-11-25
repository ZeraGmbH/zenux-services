#ifndef MOCKCLAMPPLUGGER_H
#define MOCKCLAMPPLUGGER_H

#include "i2csettings.h"
#include "sensesettings.h"

class MockClampPlugger
{
public:
    MockClampPlugger(const cSenseSettings *senseSettings, const I2cSettings *i2cSettings);
    static QByteArray getFreshEEpromData(int clampType);
    void initClampAdjMem(int clampType, QString channelAlias);
private:
    const cSenseSettings *m_senseSettings;
    const I2cSettings *m_i2cSettings;
};

#endif // MOCKCLAMPPLUGGER_H
