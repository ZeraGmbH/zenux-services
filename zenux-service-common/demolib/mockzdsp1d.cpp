#include "mockzdsp1d.h"

MockZdsp1d::MockZdsp1d() :
    MockPcbServer("zdsp1d")
{
    m_dspSettings = std::make_unique<cDSPSettings>(getConfigReader());
    setXmlSettings(XmlSettingsList{m_dspSettings.get()});

    // zdsp1d resources are to be implemented

    start();
}
