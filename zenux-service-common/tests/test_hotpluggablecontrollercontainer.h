#ifndef TEST_HOTPLUGGABLECONTROLLERCONTAINER_H
#define TEST_HOTPLUGGABLECONTROLLERCONTAINER_H

#include "testi2csettings.h"
#include "sensesettings.h"
#include "abstractfactoryi2cctrl.h"

class test_hotpluggablecontrollercontainer : public QObject
{
    Q_OBJECT
public:
    test_hotpluggablecontrollercontainer();
private slots:
    void initTestCase();
    void initTestCase_data();
    void init();

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
    std::unique_ptr<TestI2cSettings> m_i2cSettings;
    std::unique_ptr<cSenseSettings> m_senseSettings;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
};

#endif // TEST_HOTPLUGGABLECONTROLLERCONTAINER_H
