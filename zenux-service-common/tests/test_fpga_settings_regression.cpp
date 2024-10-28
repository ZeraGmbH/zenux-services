#include "test_fpga_settings_regression.h"
#include "mt310s2d.h"
#include "com5003d.h"
#include "sec1000d.h"
#include "zdspserver.h"
#include "mockfactorydevicenodepcb.h"
#include "mockfactorydevicenodesec.h"
#include "testfactorydevicenodedsp.h"
#include "mockserverparamgenerator.h"
#include "testfactoryi2cctrl.h"
#include <timemachineobject.h>
#include <tcpworkerfactory.h>
#include <QTest>

QTEST_MAIN(test_fpga_settings_regression)

void test_fpga_settings_regression::com5003d()
{
    ServerParams params = MockServerParamGenerator::createParams("com5003d");
    cCOM5003dServer server(
        std::make_unique<SettingsContainer>(params),
        std::make_shared<TestFactoryI2cCtrl>(true),
        std::make_shared<MockFactoryDeviceNodePcb>(),
        VeinTcp::TcpWorkerFactory::create());
    TimeMachineObject::feedEventLoop();

    QCOMPARE(server.getCtrlDeviceNode(), "/dev/zFPGA1reg");
}

void test_fpga_settings_regression::mt310s2d()
{
    ServerParams params = MockServerParamGenerator::createParams("mt310s2d");
    cMT310S2dServer server(
        std::make_unique<SettingsContainer>(params),
        std::make_shared<TestFactoryI2cCtrl>(true),
        std::make_shared<MockFactoryDeviceNodePcb>(),
        VeinTcp::TcpWorkerFactory::create());
    TimeMachineObject::feedEventLoop();

    QCOMPARE(server.getCtrlDeviceNode(), "/dev/zFPGA1reg");
    QCOMPARE(server.getMsgDeviceNode(), "/dev/zFPGA1msg");
}

void test_fpga_settings_regression::sec1000d()
{
    ServerParams params = MockServerParamGenerator::createParams("sec1000d");
    cSEC1000dServer server(
        std::make_unique<SettingsContainer>(params),
        std::make_shared<MockFactoryDeviceNodeSec>(),
        VeinTcp::TcpWorkerFactory::create());
    TimeMachineObject::feedEventLoop();

    QCOMPARE(server.getSecDeviceNode(), "/dev/zFPGA1ec");
}

void test_fpga_settings_regression::zdsp1d()
{
    ServerParams params = MockServerParamGenerator::createParams("zdsp1d");
    ZDspServer server(
        std::make_unique<SettingsContainer>(params),
        std::make_shared<TestFactoryDeviceNodeDsp>(),
        VeinTcp::TcpWorkerFactory::create());
    TimeMachineObject::feedEventLoop();

    QCOMPARE(server.getDspDeviceNode(), "/dev/zFPGA1dsp1");
}
