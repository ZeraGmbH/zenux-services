#include "mockmt310s2d.h"
#include "mockserverparamgenerator.h"
#include "mocki2ceepromiofactory.h"
#include "mockpcbdevicenodectrlsingleton.h"
#include "mockpcbdevicenodemessagesingleton.h"

MockMt310s2d::MockMt310s2d(AbstractFactoryI2cCtrlPtr ctrlFactory, QString alternateConfigXml)
{
    MockI2cEEpromIoFactory::enableMock();
    MockPcbDeviceNodeCtrlSingleton::enableMock();
    MockPcbDeviceNodeMessageSingleton::enableMock();

    ServerParams params = MockServerParamGenerator::createParams("mt310s2d");
    if(!alternateConfigXml.isEmpty())
        params.xmlFile = alternateConfigXml;
    std::unique_ptr<SettingsContainer> settings = std::make_unique<SettingsContainer>(params);
    m_server = std::make_unique<cMT310S2dServer>(std::move(settings), ctrlFactory);
}
