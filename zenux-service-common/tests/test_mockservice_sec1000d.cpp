#include "test_mockservice_sec1000d.h"
#include "proxy.h"
#include "secinterface.h"
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
}

void test_mockservice_sec1000d::getChannelCatSec1000d()
{
    Zera::ProxyClientPtr secClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6305);
    Zera::cSECInterface secIFace;
    secIFace.setClientSmart(secClient);

    Zera::Proxy::getInstance()->startConnectionSmart(secClient);
    TimeMachineObject::feedEventLoop();

    QSignalSpy responseSpy(&secIFace, &Zera::cSECInterface::serverAnswer);
    int msgNr = secIFace.getChannelCatalog();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("ec0;ec1;ec2;ec3;ec4;ec5;ec6;ec7;"));
}

void test_mockservice_sec1000d::connectClientThenRemoveIt()
{
    Zera::ProxyClientPtr secClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6305);
    Zera::cSECInterface secIFace;
    secIFace.setClientSmart(secClient);
    Zera::Proxy::getInstance()->startConnectionSmart(secClient);
    TimeMachineObject::feedEventLoop();

    QSignalSpy clientDisconnectionSpy(m_sec1000d.get(), &MockSec1000d::peerDisconnected);
    Zera::Proxy::getInstance()->deletePeerAndItsClients("127.0.0.1", 6305);
    TimeMachineObject::feedEventLoop();
    QCOMPARE(clientDisconnectionSpy.count(), 1);
}
