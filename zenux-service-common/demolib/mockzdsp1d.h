#ifndef MOCKZDSP1D_H
#define MOCKZDSP1D_H

#include "mockpcbserver.h"
#include "dspsettings.h"
#include <memory>

class MockZdsp1d : public MockPcbServer
{
public:
    MockZdsp1d();
private:
    std::unique_ptr<cDSPSettings> m_dspSettings;
};

#endif // MOCKZDSP1D_H
