#include "test_accumulatorinterface_mock.h"
#include "pcbinterface.h"
#include "proxy.h"
#include "reply.h"
#include <timemachineobject.h>
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>

#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_accumulatorinterface_mock)

void test_accumulatorinterface_mock::initTestCase()
{
    TimerFactoryQtForTest::enableTest();
}

void test_accumulatorinterface_mock::init()
{
    TimeMachineForTest::reset();

    m_resman = std::make_unique<ResmanRunFacade>();
    m_mt310s2d = std::make_unique<MockMt310s2dFull>();
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}

void test_accumulatorinterface_mock::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_mt310s2d = nullptr;
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_accumulatorinterface_mock::readAccumulatorStatus()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    int msgNr = m_pcbIFace->getAccumulatorStatus();

    TimeMachineForTest::getInstance()->processTimers(10000);

    QCOMPARE(responseSpy.count(), 1);
    // we are smoke test: Ignore adjustment status
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));

}

void test_accumulatorinterface_mock::readAccuStateOfCharge()
{
    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);
    int msgNr = m_pcbIFace->getAccuStateOfCharge();

    TimeMachineForTest::getInstance()->processTimers(10000);

    QCOMPARE(responseSpy.count(), 1);
    // we are smoke test: Ignore adjustment status
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
}
