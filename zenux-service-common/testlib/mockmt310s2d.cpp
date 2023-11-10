#include "mockmt310s2d.h"

MockMt310s2d::MockMt310s2d() :
    MockPcbServer("mt310s2d")
{
    m_foutSettings = std::make_unique<FOutSettings>(getConfigReader());
    m_hkInSettings = std::make_unique<HkInSettings>(getConfigReader());
    m_scInSettings = std::make_unique<ScInSettings>(getConfigReader());
    setXmlSettings(XmlSettingsList{m_foutSettings.get(), m_hkInSettings.get(), m_scInSettings.get()});

    m_foutResource = std::make_unique<FOutGroupResourceAndInterface>(getSCPIInterface(), m_foutSettings.get());
    m_hkInResource = std::make_unique<HkInGroupResourceAndInterface>(getSCPIInterface(), m_hkInSettings.get());
    m_scInResource = std::make_unique<ScInGroupResourceAndInterface>(getSCPIInterface(), m_scInSettings.get());
    setResources(ResourcesList{m_foutResource.get(), m_hkInResource.get(), m_scInResource.get()});
    start();
}
