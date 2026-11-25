#include "test_hard_overload_interrupt.h"
#include <mocktcpnetworkfactory.h>
#include <timerfactoryqtfortest.h>

#include <QTest>

QTEST_MAIN(test_hard_overload_interrupt);

void test_hard_overload_interrupt::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
    m_tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(m_tcpNetworkFactory);
}

void test_hard_overload_interrupt::initTestCase_data()
{

}
