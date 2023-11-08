#include "test_servicemock.h"
#include "mockmt310s2d.h"
#include <QAbstractEventDispatcher>
#include <QTest>

QTEST_MAIN(test_servicemock)

void test_servicemock::init()
{
    m_resMan = std::make_shared<ResmanRunFacade>();
    feedEventLoop();
}

void test_servicemock::runMockMt310s2d()
{
    MockMt310s2d mtMockService;
    mtMockService.start();
    feedEventLoop();
    QVERIFY(mtMockService.areAllResourcesConnected());
}

void test_servicemock::feedEventLoop()
{
    while(QCoreApplication::eventDispatcher()->processEvents(QEventLoop::AllEvents));
}
