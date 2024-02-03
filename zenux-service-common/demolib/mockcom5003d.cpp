#include "mockcom5003d.h"
#include "mockserverparamgenerator.h"
#include "mocki2ceepromiofactory.h"
#include "mockpcbdevicenodectrlsingleton.h"
#include "mockpcbdevicenodemessagesingleton.h"

MockCom5003d::MockCom5003d(AbstractFactoryI2cCtrlPtr ctrlFactory, QString alternateConfigXml)
{
    MockI2cEEpromIoFactory::enableMock();
    MockPcbDeviceNodeCtrlSingleton::enableMock();
    MockPcbDeviceNodeMessageSingleton::enableMock();

    ServerParams params = MockServerParamGenerator::createParams("com5003d");
    if(!alternateConfigXml.isEmpty())
        params.xmlFile = alternateConfigXml;
    std::unique_ptr<SettingsContainer> settings = std::make_unique<SettingsContainer>(params);
    m_server = std::make_unique<cCOM5003dServer>(std::move(settings), ctrlFactory);
}
