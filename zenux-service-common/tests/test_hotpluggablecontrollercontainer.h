#ifndef TEST_HOTPLUGGABLECONTROLLERCONTAINER_H
#define TEST_HOTPLUGGABLECONTROLLERCONTAINER_H

#include "sensesettings.h"
#include <QObject>

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
private:
    Zera::XMLConfig::cReader m_configReader;
    cSenseSettings m_senseSettings;
};

#endif // TEST_HOTPLUGGABLECONTROLLERCONTAINER_H
