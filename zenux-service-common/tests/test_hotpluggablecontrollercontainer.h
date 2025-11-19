#ifndef TEST_HOTPLUGGABLECONTROLLERCONTAINER_H
#define TEST_HOTPLUGGABLECONTROLLERCONTAINER_H

#include "mockmt310s2d.h"
#include "testi2csettings.h"
#include "sensesettings.h"
#include "abstractfactoryi2cctrl.h"
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
    void mt310s2AddI1();
    void mt310s2AddI1I2();
    void mt310s2AddI1Twice();
    void mt310s2AddI1I2RemoveI2();
    void mt310s2AddI1CheckI2cSettings();
    void mt310s2AddI1I2I3IAuxCheckMuxSettings();
    void mt310s2AddI1CheckSignals();
    void mt310s2AddI1I2CheckSignalsImmediate();
    void mt310s2AddI1I2CheckSignalsDelayed();
    void mt310s2AddI1AndRemoveBeforeFinish();
    void mt310s2AddI1AndAddI2BeforeFinish();
    void mt310s2AddClampNoController();

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
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;

    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mt310s2d;

};

#endif // TEST_HOTPLUGGABLECONTROLLERCONTAINER_H
