#ifndef TEST_HOTPLUGGABLECONTROLLERCONTAINER_H
#define TEST_HOTPLUGGABLECONTROLLERCONTAINER_H

#include "mockmt310s2d.h"
#include "testfactoryi2cctrl.h"
#include "testi2csettings.h"
#include "sensesettings.h"
#include <resmanrunfacade.h>

class test_hotpluggablecontrollercontainer : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void initTestCase_data();
    void init();
    void cleanup();

    void initNoController();
    void mt310s2AllVoltageNotPluggable();
    void addI1Controller();
    void addI1I2Controllers();
    void addI1ControllerTwice();
    void addI1I2ControllerRemoveI2();
    void addI1ControllerCheckI2cSettings();
    void addI1I2I3IAuxControllerCheckMuxSettings();
    void addI1ControllerBootloaderActiveCheckSignals();
    void addI1I2ControllerApplicationActiveCheckSignals();
    void addI1I2ControllerBootloaderActiveCheckSignals();
    void addI1ControllerBootloaderActiveAndRemoveBeforeStartApplicationFinish();
    void addI1ControllerBootloaderActiveAndAddI2ControllerBeforeI1StartApplicationFinish();
    void clampControllerSequence();

    void mt310s2AddI1InstrumentDetectsMt650e();
    void mt310s2AddI1InstrumentDefaultsEmob();
private:
    quint16 getChannelPlugMask(const QString &channelAlias);
    QString getChannelMName(const QString &channelAlias);
    quint8 getChannelMuxChannel(const QString &channelAlias);
    void createServers();
    Zera::XMLConfig::cReader m_configReader;
    std::unique_ptr<TestI2cSettings> m_i2cSettings;
    std::unique_ptr<cSenseSettings> m_senseSettings;

    TestFactoryI2cCtrlPtr m_ctrlFactory;
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mt310s2d;

};

#endif // TEST_HOTPLUGGABLECONTROLLERCONTAINER_H
