#include "test_regression_scpi_zdsp1d.h"
#include "testfactoryzdspsupport.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include "zscpi_response_definitions.h"
#include <xmldocumentcompare.h>
#include <testloghelpers.h>
#include <timerfactoryqtfortest.h>
#include <timemachineobject.h>
#include <mocktcpnetworkfactory.h>
#include <QTest>

QTEST_MAIN(test_regression_scpi_zdsp1d);

void test_regression_scpi_zdsp1d::initTestCase()
{
    qputenv("QT_FATAL_CRITICALS", "1");
    TimerFactoryQtForTest::enableTest();
    m_tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(m_tcpNetworkFactory);
}

void test_regression_scpi_zdsp1d::init()
{
    m_server = std::make_unique<MockZdsp1d>(std::make_shared<TestFactoryZdspSupport>(), m_tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6310, m_tcpNetworkFactory);
    m_dspIFace = std::make_unique<Zera::cDSPInterface>();
    m_dspIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

void test_regression_scpi_zdsp1d::cleanup()
{
    m_dspIFace = nullptr;
    m_proxyClient = nullptr;
    m_server = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_scpi_zdsp1d::serverUp()
{
    QString ret = ScpiSingleTransactionBlocked::query("SYSTEM:VERSION:SERVER?", 6310);
    QCOMPARE(ret, "zdsp1d V1.11");
}

void test_regression_scpi_zdsp1d::dumpScpi()
{
    QString expected = TestLogHelpers::loadFile("://scpi-dump.xml");
    QString dumped = ScpiSingleTransactionBlocked::query("SYSTEM:INTERFACE:READ?", 6310);
    XmlDocumentCompare compare;
    bool ok = compare.compareXml(dumped, expected);
    if(!ok)
        TestLogHelpers::compareAndLogOnDiff(expected, dumped);
    QVERIFY(ok);
}

void test_regression_scpi_zdsp1d::superClientResponseAck()
{
    quint8 replyReceived = -1;
    connect(m_dspIFace.get(), &AbstractServerInterface::serverAnswer, [&](quint32, quint8 reply, const QVariant&) {
        replyReceived = reply;
    });
    m_dspIFace->scpiCommand("SYSTEM:DSP:DSPSUPERCLIENT;");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(replyReceived, ZSCPI::ack);
}

void test_regression_scpi_zdsp1d::superClientResponseTwiceAckNak()
{
    quint8 replyReceived = -1;
    connect(m_dspIFace.get(), &AbstractServerInterface::serverAnswer, [&](quint32, quint8 reply, const QVariant&) {
        replyReceived = reply;
    });
    m_dspIFace->scpiCommand("SYSTEM:DSP:DSPSUPERCLIENT;");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(replyReceived, ZSCPI::ack);

    m_dspIFace->scpiCommand("SYSTEM:DSP:DSPSUPERCLIENT;");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(replyReceived, ZSCPI::nak);
}

void test_regression_scpi_zdsp1d::superClientResponseTwiceAckConnectionClosed()
{
    QString ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:DSP:DSPSUPERCLIENT", "", 6310); // connection closed
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);

    ret = ScpiSingleTransactionBlocked::cmd("SYSTEM:DSP:DSPSUPERCLIENT", "", 6310);
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_regression_scpi_zdsp1d::superClientResponseSetAckUnloadDspSetAck()
{
    quint8 replyReceived = -1;
    connect(m_dspIFace.get(), &AbstractServerInterface::serverAnswer, [&](quint32, quint8 reply, const QVariant&) {
        replyReceived = reply;
    });
    m_dspIFace->scpiCommand("SYSTEM:DSP:DSPSUPERCLIENT;");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(replyReceived, ZSCPI::ack);

    replyReceived = -1;
    m_dspIFace->scpiCommand("MEMORY:CLALL;");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(replyReceived, ZSCPI::ack);

    replyReceived = -1;
    m_dspIFace->scpiCommand("SYSTEM:DSP:DSPSUPERCLIENT;");
    TimeMachineObject::feedEventLoop();
    QCOMPARE(replyReceived, ZSCPI::ack);
}
