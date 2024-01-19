#include "mockmt310s2d.h"
#include "serverparamgenerator.h"
#include "mocki2ceepromiofactory.h"
#include "mockpcbdevicenodectrlsingleton.h"
#include "mockpcbdevicenodemessagesingleton.h"

MockMt310s2d::MockMt310s2d(FactoryControllerAbstractPtr ctrlFactory, QString alternateConfigXml)
{
    MockI2cEEpromIoFactory::enableMock();
    MockPcbDeviceNodeCtrlSingleton::enableMock();
    MockPcbDeviceNodeMessageSingleton::enableMock();

    ServerParams params = ServerParamGenerator::createParams("mt310s2d");
    if(!alternateConfigXml.isEmpty())
        params.xmlFile = alternateConfigXml;
    std::shared_ptr<SettingsForDeviceServer> settings = std::make_shared<SettingsForDeviceServer>(params);
    m_server = std::make_unique<cMT310S2dServer>(settings, ctrlFactory);
}
