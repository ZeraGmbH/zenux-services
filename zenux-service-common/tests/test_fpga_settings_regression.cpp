#include "test_fpga_settings_regression.h"
#include "mt310s2d.h"
#include "com5003d.h"
#include "sec1000d.h"
#include "zdspserver.h"
#include "serverparamgenerator.h"
#include "testfactoryi2cctrl.h"
#include "mockpcbdevicenodectrlsingleton.h"
#include "mockpcbdevicenodemessagesingleton.h"
#include "mocksecdevicenodesingleton.h"
#include "mockdspdevicenodesingleton.h"
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_fpga_settings_regression)

// At the time of writing all servers need accesible device nodes so we enable
// mocks. Otherwise servers kill themseves early see <server>::doCloseServer()
// and we might access data which was already deleted

void test_fpga_settings_regression::com5003d()
{
    MockPcbDeviceNodeCtrlSingleton::enableMock();

    ServerParams params = ServerParamGenerator::createParams("com5003d");
    std::shared_ptr<SettingsContainer> settings = std::make_shared<SettingsContainer>(params);

    cCOM5003dServer server(settings, std::make_shared<TestFactoryI2cCtrl>(true));
    TimeMachineObject::feedEventLoop();

    QCOMPARE(server.getCtrlDeviceNode(), "/dev/zFPGA1reg");
}

void test_fpga_settings_regression::mt310s2d()
{
    MockPcbDeviceNodeCtrlSingleton::enableMock();
    MockPcbDeviceNodeMessageSingleton::enableMock();

    ServerParams params = ServerParamGenerator::createParams("mt310s2d");
    std::shared_ptr<SettingsContainer> settings = std::make_shared<SettingsContainer>(params);

    cMT310S2dServer server(settings, std::make_shared<TestFactoryI2cCtrl>(true));
    TimeMachineObject::feedEventLoop();

    QCOMPARE(server.getCtrlDeviceNode(), "/dev/zFPGA1reg");
    QCOMPARE(server.getMsgDeviceNode(), "/dev/zFPGA1msg");
}

void test_fpga_settings_regression::sec1000d()
{
    MockSecDeviceNodeSingleton::enableMock();
    cSEC1000dServer server(ServerParamGenerator::createParams("sec1000d"));
    TimeMachineObject::feedEventLoop();

    QCOMPARE(server.getSecDeviceNode(), "/dev/zFPGA1ec");
}

void test_fpga_settings_regression::zdsp1d()
{
    MockDspDeviceNodeSingleton::enableMock();
    ZDspServer server(ServerParamGenerator::createParams("zdsp1d"));
    TimeMachineObject::feedEventLoop();

    QCOMPARE(server.getDspDeviceNode(), "/dev/zFPGA1dsp1");
}
