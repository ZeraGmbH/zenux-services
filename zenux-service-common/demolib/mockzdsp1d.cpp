#include "mockzdsp1d.h"
#include "mockfactorydevicenodedsp.h"
#include "mockserverparamgenerator.h"

MockZdsp1d::MockZdsp1d(QString alternateConfigXml, int dspMagicId)
{
    ServerParams params = MockServerParamGenerator::createParams("zdsp1d");
    if(!alternateConfigXml.isEmpty())
        params.xmlFile = alternateConfigXml;
    std::unique_ptr<SettingsContainer> settings = std::make_unique<SettingsContainer>(params);
    m_server = std::make_unique<ZDspServer>(std::move(settings), std::make_shared<MockFactoryDeviceNodeDsp>(dspMagicId));
}
