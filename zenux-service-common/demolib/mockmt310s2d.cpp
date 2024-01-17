#include "mockmt310s2d.h"
#include "mockfactoryi2cctrl.h"

MockMt310s2d::MockMt310s2d() :
    MockPcbServer("mt310s2d")
{
    m_finSettings = std::make_unique<FInSettings>(getConfigReader());
    m_foutSettings = std::make_unique<FOutSettings>(getConfigReader());
    m_hkInSettings = std::make_unique<HkInSettings>(getConfigReader());
    m_scInSettings = std::make_unique<ScInSettings>(getConfigReader());
    m_samplingSettings = std::make_unique<SamplingSettings>(getConfigReader());
    setXmlSettings(XmlSettingsList{m_finSettings.get(), m_foutSettings.get(), m_hkInSettings.get(), m_scInSettings.get(), m_samplingSettings.get()});

    m_finResource = std::make_unique<FInGroupResourceAndInterface>(getSCPIInterface(), m_finSettings.get());
    m_foutResource = std::make_unique<FOutGroupResourceAndInterface>(getSCPIInterface(), m_foutSettings.get());
    m_hkInResource = std::make_unique<HkInGroupResourceAndInterface>(getSCPIInterface(), m_hkInSettings.get());
    m_scInResource = std::make_unique<ScInGroupResourceAndInterface>(getSCPIInterface(), m_scInSettings.get());
    m_samplingInterface = std::make_unique<cSamplingInterface>(getSCPIInterface(), m_samplingSettings.get(), std::make_shared<MockFactoryI2cCtrl>(true));
    setResources(ResourcesList{m_finResource.get(), m_foutResource.get(), m_hkInResource.get(), m_scInResource.get(), m_samplingInterface.get()});
    start();
}
