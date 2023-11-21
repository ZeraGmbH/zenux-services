#include "mockcom5003d.h"

MockCom5003d::MockCom5003d(QString sysrootDir) :
    MockPcbServer("com5003d", sysrootDir)
{
    m_foutSettings = std::make_unique<FOutSettings>(getConfigReader());
    m_hkInSettings = std::make_unique<HkInSettings>(getConfigReader());
    m_scInSettings = std::make_unique<ScInSettings>(getConfigReader());
    m_samplingSettings = std::make_unique<SamplingSettings>(getConfigReader());
    setXmlSettings(XmlSettingsList{m_foutSettings.get(), m_hkInSettings.get(), m_scInSettings.get(), m_samplingSettings.get()});

    m_foutResource = std::make_unique<FOutGroupResourceAndInterface>(getSCPIInterface(), m_foutSettings.get());
    m_hkInResource = std::make_unique<HkInGroupResourceAndInterface>(getSCPIInterface(), m_hkInSettings.get());
    m_scInResource = std::make_unique<ScInGroupResourceAndInterface>(getSCPIInterface(), m_scInSettings.get());
    m_samplingInterface = std::make_unique<cSamplingInterface>(getSCPIInterface(), m_samplingSettings.get());
    setResources(ResourcesList{m_foutResource.get(), m_hkInResource.get(), m_scInResource.get(), m_samplingInterface.get()});
    start();
}
