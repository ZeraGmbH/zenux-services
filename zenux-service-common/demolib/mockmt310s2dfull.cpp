#include "mockmt310s2dfull.h"
#include "mockpcbserver.h"
#include "mocki2ceepromiofactory.h"
#include "pcbdevicenodectrlsingletonmock.h"
#include "pcbdevicenodemessagesingletonmock.h"

MockMt310s2dFull::MockMt310s2dFull(std::shared_ptr<SettingsForDeviceServer> settings,
                                   AtmelCtrlFactoryInterfacePrt ctrlFactory) :
    cMT310S2dServer(settings, ctrlFactory, MockPcbServer::createParams("mt310s2d"))
{
    MockI2cEEpromIoFactory::enableMock();
    PcbDeviceNodeCtrlSingletonMock::enableMock();
    PcbDeviceNodeMessageSingletonMock::enableMock();
}
