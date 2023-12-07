#include "test_mockservice_sec1000d.h"
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
    m_proxy = std::make_unique<ProxyForTest>();
}

void test_mockservice_sec1000d::cleanup()
{
    m_sec1000d = nullptr;
    TimeMachineObject::feedEventLoop();
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
    m_proxy = nullptr;
}

Zera::cSECInterfacePtr test_mockservice_sec1000d::getSecInterface()
{
    Zera::cSECInterfacePtr secInterface = std::make_unique<Zera::cSECInterface>();
    Zera::ProxyClientPtr secClient = m_proxy->getConnectionSmart("127.0.0.1", 6305);
    secInterface->setClientSmart(secClient);
    m_proxy->startConnectionSmart(secClient);
    TimeMachineObject::feedEventLoop();
    return secInterface;
}

void test_mockservice_sec1000d::getChannelCatSec1000d()
{
    Zera::cSECInterfacePtr secInterface = getSecInterface();
    QSignalSpy responseSpy(secInterface.get(), &Zera::cSECInterface::serverAnswer);
    int msgNr = secInterface->getChannelCatalog();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ec0;ec1;ec2;ec3;ec4;ec5;ec6;ec7;"));
}
