#ifndef MOCKCOM5003D_H
#define MOCKCOM5003D_H

#include "mockpcbserver.h"
#include "foutgroupresourceandinterface.h"
#include "hkingroupresourceandinterface.h"
#include "scingroupresourceandinterface.h"
#include "samplinginterface.h"
#include <memory>

class MockCom5003d : public MockPcbServer
{
public:
    MockCom5003d(QString sysrootDir);
private:
    std::unique_ptr<FOutSettings> m_foutSettings;
    std::unique_ptr<HkInSettings> m_hkInSettings;
    std::unique_ptr<ScInSettings> m_scInSettings;
    std::unique_ptr<SamplingSettings> m_samplingSettings;
    std::unique_ptr<FOutGroupResourceAndInterface> m_foutResource;
    std::unique_ptr<HkInGroupResourceAndInterface> m_hkInResource;
    std::unique_ptr<ScInGroupResourceAndInterface> m_scInResource;
    std::unique_ptr<cSamplingInterface> m_samplingInterface;
};

#endif // MOCKCOM5003D_H
