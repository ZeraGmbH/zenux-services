#include "mockcom5003d.h"
#include "serverparamgenerator.h"
#include "mocki2ceepromiofactory.h"
#include "mockpcbdevicenodectrlsingleton.h"
#include "mockpcbdevicenodemessagesingleton.h"

MockCom5003d::MockCom5003d(FactoryControllerAbstractPtr ctrlFactory, QString alternateConfigXml)
{
    MockI2cEEpromIoFactory::enableMock();
    MockPcbDeviceNodeCtrlSingleton::enableMock();
    MockPcbDeviceNodeMessageSingleton::enableMock();

    ServerParams params = ServerParamGenerator::createParams("com5003d");
    if(!alternateConfigXml.isEmpty())
        params.xmlFile = alternateConfigXml;
    std::shared_ptr<SettingsForDeviceServer> settings = std::make_shared<SettingsForDeviceServer>(params);
    m_server = std::make_unique<cCOM5003dServer>(settings, ctrlFactory);
}
