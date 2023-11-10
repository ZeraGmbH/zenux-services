#ifndef MOCKMT310S2D_H
#define MOCKMT310S2D_H

#include "mockpcbserver.h"
#include "foutgroupresourceandinterface.h"
#include "hkingroupresourceandinterface.h"
#include "scingroupresourceandinterface.h"
#include <memory>

class MockMt310s2d : public MockPcbServer
{
public:
    MockMt310s2d();

private:
    std::unique_ptr<FOutSettings> m_foutSettings;
    std::unique_ptr<HkInSettings> m_hkInSettings;
    std::unique_ptr<ScInSettings> m_scInSettings;
    std::unique_ptr<FOutGroupResourceAndInterface> m_foutResource;
    std::unique_ptr<HkInGroupResourceAndInterface> m_hkInResource;
    std::unique_ptr<ScInGroupResourceAndInterface> m_scInResource;
};

#endif // MOCKMT310S2D_H
