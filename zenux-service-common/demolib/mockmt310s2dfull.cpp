#include "mockmt310s2dfull.h"
#include "mockpcbserver.h"
#include "mocki2ceepromiofactory.h"
#include "mockpcbdevicenodectrlsingleton.h"
#include "mockpcbdevicenodemessagesingleton.h"
#include "mockfactoryi2cctrl.h"

MockMt310s2dFull::MockMt310s2dFull(QString alternateConfigXml)
{
    MockI2cEEpromIoFactory::enableMock();
    MockPcbDeviceNodeCtrlSingleton::enableMock();
    MockPcbDeviceNodeMessageSingleton::enableMock();

    ServerParams params = MockPcbServer::createParams("mt310s2d");
    if(!alternateConfigXml.isEmpty())
        params.xmlFile = alternateConfigXml;
    std::shared_ptr<SettingsForDeviceServer> settings = std::make_shared<SettingsForDeviceServer>(params);
    m_server = std::make_unique<cMT310S2dServer>(settings, std::make_shared<MockFactoryI2cCtrl>(true));
}
