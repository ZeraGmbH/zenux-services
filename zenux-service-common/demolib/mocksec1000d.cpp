#include "mocksec1000d.h"
#include "mockserverparamgenerator.h"
#include "mocksecdevicenodesingleton.h"

MockSec1000d::MockSec1000d(QString alternateConfigXml)
{
    MockSecDeviceNodeSingleton::enableMock();

    ServerParams params = MockServerParamGenerator::createParams("sec1000d");
    if(!alternateConfigXml.isEmpty())
        params.xmlFile = alternateConfigXml;
    std::unique_ptr<SettingsContainer> settings = std::make_unique<SettingsContainer>(params);
    m_server = std::make_unique<cSEC1000dServer>(std::move(settings));
}
