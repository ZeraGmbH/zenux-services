#include "test_regression_adjustment_mt310s2.h"
#include "clampfactorytest.h"
#include "proxy.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_regression_adjustment_mt310s2);

void test_regression_adjustment_mt310s2::initTestCase()
{
    ClampFactoryTest::enableTest();
}

void test_regression_adjustment_mt310s2::init()
{
    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_mockServer = std::make_unique<MockForSenseInterfaceMt310s2>();
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}

void test_regression_adjustment_mt310s2::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_mockServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}
