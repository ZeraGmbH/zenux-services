#include "test_accumulatorinterface_mock.h"
#include "pcbinterface.h"
#include "proxy.h"
#include "reply.h"
#include "testfactoryi2cctrl.h"
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

}

void test_accumulatorinterface_mock::cleanup()
{
    m_pcbIFace = nullptr;
    m_pcbClient = nullptr;
    m_mt310s2d = nullptr;
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
}

constexpr int ACCU_POLLING_PERIOD_MS = 1000;

void test_accumulatorinterface_mock::readAccuStatusAccuDisabled()
{
    setupServers(":/accu_disabled.xml");
    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);

    TimeMachineForTest::getInstance()->processTimers(ACCU_POLLING_PERIOD_MS);

    int msgNr = m_pcbIFace->getAccumulatorStatus();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant(0)); // pcb-interface return integer
}

void test_accumulatorinterface_mock::readAccuStateOfChargeAccuDisabled()
{
    setupServers(":/accu_disabled.xml");
    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);

    TimeMachineForTest::getInstance()->processTimers(ACCU_POLLING_PERIOD_MS);

    int msgNr = m_pcbIFace->getAccuStateOfCharge();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant(0));
}

void test_accumulatorinterface_mock::readAccuStatusAccuEnabled()
{
    setupServers(":/accu_enabled.xml");
    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);

    TimeMachineForTest::getInstance()->processTimers(ACCU_POLLING_PERIOD_MS);

    int msgNr = m_pcbIFace->getAccumulatorStatus();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant(0));
}

void test_accumulatorinterface_mock::readAccuStateOfChargeAccuEnabled()
{
    setupServers(":/accu_enabled.xml");
    QSignalSpy responseSpy(m_pcbIFace.get(), &Zera::cPCBInterface::serverAnswer);

    TimeMachineForTest::getInstance()->processTimers(ACCU_POLLING_PERIOD_MS);

    int msgNr = m_pcbIFace->getAccuStateOfCharge();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant(37));
}

void test_accumulatorinterface_mock::setupServers(QString configFileXml)
{
    m_resman = std::make_unique<ResmanRunFacade>();
    m_mt310s2d = std::make_unique<MockMt310s2d>(std::make_shared<TestFactoryI2cCtrl>(true), configFileXml);
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    m_pcbIFace = std::make_unique<Zera::cPCBInterface>();
    m_pcbIFace->setClientSmart(m_pcbClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}
