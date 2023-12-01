#include "test_mockservice_zdsp1d.h"
#include "mockzdsp1d.h"
#include "resmanrunfacade.h"
#include "proxy.h"
#include "dspinterface.h"
#include "reply.h"
#include <QAbstractEventDispatcher>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(test_mockservice_zdsp1d)

void test_mockservice_zdsp1d::getVersion()
{
    ResmanRunFacade resman;
    feedEventLoop();

    MockZdsp1d zdsp1d;
    feedEventLoop();

    Zera::ProxyClientPtr dspClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6310);
    Zera::cDSPInterface dspIFace;
    dspIFace.setClientSmart(dspClient);

    Zera::Proxy::getInstance()->startConnectionSmart(dspClient);
    feedEventLoop();

    QSignalSpy responseSpy(&dspIFace, &Zera::cDSPInterface::serverAnswer);
    int msgNr = dspIFace.readDeviceVersion();
    feedEventLoop();

    QCOMPARE(responseSpy.count(), 1);
    QCOMPARE(responseSpy[0][0], QVariant(msgNr));
    QCOMPARE(responseSpy[0][1], QVariant(ack));
    QCOMPARE(responseSpy[0][2], QVariant("DSPLCA: V0.0;DSP V0.00"));
}

void test_mockservice_zdsp1d::feedEventLoop()
{
    while(QCoreApplication::eventDispatcher()->processEvents(QEventLoop::AllEvents));
}
