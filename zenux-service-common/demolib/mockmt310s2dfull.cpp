#include "mockmt310s2dfull.h"
#include "mockpcbserver.h"
#include "mocki2ceepromiofactory.h"
#include "pcbdevicenodectrlsingletonmock.h"
#include "pcbdevicenodemessagesingletonmock.h"
#include "mockatmelctrlfactory.h"

MockMt310s2dFull::MockMt310s2dFull()
{
    MockI2cEEpromIoFactory::enableMock();
    PcbDeviceNodeCtrlSingletonMock::enableMock();
    PcbDeviceNodeMessageSingletonMock::enableMock();

    ServerParams params = MockPcbServer::createParams("mt310s2d");
    std::shared_ptr<SettingsForDeviceServer> settings = std::make_shared<SettingsForDeviceServer>(params);
    m_server = std::make_unique<cMT310S2dServer>(settings, std::make_shared<MockAtmelCtrlFactory>(true), params);
}
