#include "test_mockservice_sec1000d.h"
#include "proxy.h"
#include "reply.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_mockservice_sec1000d)

void test_mockservice_sec1000d::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_mockservice_sec1000d::init()
{
    m_resman = std::make_unique<ResmanRunFacade>();
    TimeMachineObject::feedEventLoop();
    m_sec1000d = std::make_unique<MockSec1000d>();
    TimeMachineObject::feedEventLoop();
}

void test_mockservice_sec1000d::cleanup()
{
    m_sec1000d = nullptr;
    TimeMachineObject::feedEventLoop();
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
    m_secIFace = nullptr;
}

void test_mockservice_sec1000d::createSecClientInterface()
{
    Zera::ProxyClientPtr secClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6305);
    m_secIFace = std::make_unique<Zera::cSECInterface>();
    m_secIFace->setClientSmart(secClient);
    Zera::Proxy::getInstance()->startConnectionSmart(secClient);
    TimeMachineObject::feedEventLoop();
}

void test_mockservice_sec1000d::getChannelCatSec1000d()
{
    createSecClientInterface();
    QSignalSpy responseSpy(m_secIFace.get(), &Zera::cSECInterface::serverAnswer);
    int msgNr = m_secIFace->getChannelCatalog();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ec0;ec1;ec2;ec3;ec4;ec5;ec6;ec7;"));
}

void test_mockservice_sec1000d::freeSecResourcesWhenClientDisconnects()
{
    createSecClientInterface();
    QSignalSpy responseSpy1(m_secIFace.get(), &Zera::cSECInterface::serverAnswer);
    m_secIFace->setECalcUnit(4); //max 4 can be set at a time
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy1.count(), 1);
    QCOMPARE(responseSpy1[0][1], QVariant(ack));

    m_secIFace->setECalcUnit(4);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy1.count(), 2);
    QCOMPARE(responseSpy1[1][1], QVariant(ack));

    Zera::Proxy::getInstance()->deletePeerAndItsClients("127.0.0.1", 6305);
    TimeMachineObject::feedEventLoop();

    createSecClientInterface();
    QSignalSpy responseSpy2(m_secIFace.get(), &Zera::cSECInterface::serverAnswer);
    m_secIFace->setECalcUnit(4);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(responseSpy2.count(), 1);
    QCOMPARE(responseSpy2[0][1], QVariant(ack));
}
