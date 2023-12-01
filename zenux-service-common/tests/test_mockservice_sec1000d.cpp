#include "test_mockservice_sec1000d.h"
#include "mocksec1000d.h"
#include "resmanrunfacade.h"
#include "proxy.h"
#include "secinterface.h"
#include "reply.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_mockservice_sec1000d)

void test_mockservice_sec1000d::getChannelCatSec1000d()
{
    ResmanRunFacade resman;
    MockSec1000d mockSec1000d;
    TimeMachineObject::feedEventLoop();

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
