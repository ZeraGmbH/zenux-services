#include "mocksec1000d.h"

MockSec1000d::MockSec1000d(QString sysrootDir) :
    MockPcbServer("sec1000d", sysrootDir)
{
    m_secSettings = std::make_unique<SecCalculatorSettings>(getConfigReader());
    m_secInputSettings = std::make_unique<SecInputSettings>(getConfigReader());
    setXmlSettings(XmlSettingsList{m_secSettings.get(), m_secInputSettings.get()});
    m_secResource = std::make_unique<SecGroupResourceAndInterface>(-1, m_secSettings.get(), m_secInputSettings.get(), nullptr);
    setResources(ResourcesList{m_secResource.get()});
    start();
}
