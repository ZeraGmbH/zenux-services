#ifndef TEST_HOTPLUGGABLECONTROLLERCONTAINER_H
#define TEST_HOTPLUGGABLECONTROLLERCONTAINER_H

#include "i2csettingsfortest.h"
#include "sensesettings.h"

class test_hotpluggablecontrollercontainer : public QObject
{
    Q_OBJECT
public:
    test_hotpluggablecontrollercontainer();
private slots:
    void initTestCase();
    void initNoController();
    void mt310s2AllVoltageNotPluggable();
    void mt310s2AddI1();
    void mt310s2AddI1I2();
    void mt310s2AddI1Twice();
    void mt310s2AddI1I2AddI1();
    void mt310s2AddI1CheckI2cSettings();
    void mt310s2AddI1I2I3IAuxCheckMuxSettings();
    void mt310s2AddI1CheckSignals();
    void mt310s2AddI1I2CheckSignalsImmediate();
    void mt310s2AddI1I2CheckSignalsDelayed();
    void mt310s2AddI1AndRemoveBeforeFinish();
    void mt310s2AddI1AndAddI2BeforeFinish();
    void mt310s2AddClampNoController();


private:
    Zera::XMLConfig::cReader m_configReader;
    I2cSettingsForTest m_i2cSettings;
    cSenseSettings m_senseSettings;
};

#endif // TEST_HOTPLUGGABLECONTROLLERCONTAINER_H
