#include "mockzdsp1d.h"
#include "mockserverparamgenerator.h"
#include "mockdspdevicenodesingleton.h"

MockZdsp1d::MockZdsp1d(QString alternateConfigXml, int dspMagicId)
{
    MockDspDeviceNodeSingleton::enableMock(dspMagicId);

    ServerParams params = MockServerParamGenerator::createParams("zdsp1d");
    if(!alternateConfigXml.isEmpty())
        params.xmlFile = alternateConfigXml;
    std::unique_ptr<SettingsContainer> settings = std::make_unique<SettingsContainer>(params);
    m_server = std::make_unique<ZDspServer>(std::move(settings));
}
