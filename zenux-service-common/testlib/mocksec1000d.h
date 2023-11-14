#ifndef MOCKSEC1000D_H
#define MOCKSEC1000D_H

#include "mockpcbserver.h"
#include "secgroupresourceandinterface.h"
#include <memory>

class MockSec1000d : public MockPcbServer
{
public:
    MockSec1000d();
private:
    std::unique_ptr<SecCalculatorSettings> m_secSettings;
    std::unique_ptr<SecInputSettings> m_secInputSettings;
    std::unique_ptr<SecGroupResourceAndInterface> m_secResource;
};

#endif // MOCKSEC1000D_H
