#include "test_mockservice_com5003d.h"
#include "proxy.h"
#include "reply.h"
#include "pcbinterface.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_mockservice_com5003d)

void test_mockservice_com5003d::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
}

void test_mockservice_com5003d::init()
{
    m_resman = std::make_unique<ResmanRunFacade>();
    TimeMachineObject::feedEventLoop();
    m_com5003d = std::make_unique<MockCom5003d>();
    TimeMachineObject::feedEventLoop();
}

void test_mockservice_com5003d::cleanup()
{
    m_com5003d = nullptr;
    TimeMachineObject::feedEventLoop();
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_mockservice_com5003d::getFoutCat()
{
    Zera::ProxyClientPtr pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    Zera::cPCBInterface pcbIFace;
    pcbIFace.setClientSmart(pcbClient);

    Zera::Proxy::getInstance()->startConnectionSmart(pcbClient);
    TimeMachineObject::feedEventLoop();

    QSignalSpy responseSpy(&pcbIFace, &Zera::cPCBInterface::serverAnswer);
    int msgNr = pcbIFace.getSourceCat();
    TimeMachineObject::feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("fo0;fo1;fo2;fo3"));
}
