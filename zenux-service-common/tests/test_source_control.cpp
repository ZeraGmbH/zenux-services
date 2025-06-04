#include "test_source_control.h"
#include "jsonstructapi.h"
#include "proxy.h"
#include "proxyclient.h"
#include "zscpi_response_definitions.h"
#include "testfactoryi2cctrl.h"
#include <abstracttcpnetworkfactory.h>
#include <mocktcpnetworkfactory.h>
#include <timemachineobject.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTest>

QTEST_MAIN(test_source_control)

void test_source_control::init()
{
    m_lastReply = 0xFF;
    m_lastAnswer = QVariant();
}

void test_source_control::cleanup()
{
    m_pcbInterface.reset();
    TimeMachineObject::feedEventLoop();
    m_mtXXXs2d.reset();
    TimeMachineObject::feedEventLoop();
    m_resman.reset();
    TimeMachineObject::feedEventLoop();
}

void test_source_control::mt310s2Capabilities()
{
    setupServerAndClient("mt310s2d");
    m_pcbInterface->scpiCommand("UISRC:CAPABILITIES?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(m_lastReply, ZSCPI::nak);
    QCOMPARE(m_lastAnswer, ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_source_control::mt581s2Capabilities()
{
    setupServerAndClient("mt581s2d");
    m_pcbInterface->scpiCommand("UISRC:CAPABILITIES?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(m_lastReply, ZSCPI::ack);
    QJsonObject capabilities = QJsonDocument::fromJson(m_lastAnswer.toString().toUtf8()).object();
    QVERIFY(!capabilities.isEmpty());
    JsonStructApi structApi(capabilities);;
    QCOMPARE(structApi.getDeviceName(), "MT581s2");
}

void test_source_control::setupServerAndClient(const QString &deviceD)
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_mtXXXs2d = std::make_unique<MockMt310s2d>(std::make_shared<TestFactoryI2cCtrl>(true), tcpNetworkFactory, deviceD);
    TimeMachineObject::feedEventLoop();

    Zera::ProxyClientPtr proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    m_pcbInterface = std::make_unique<TinyServerScpiCmdInterface>(proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(proxyClient);
    TimeMachineObject::feedEventLoop();

    connect(m_pcbInterface.get(), &TinyServerScpiCmdInterface::serverAnswer, this, [&](quint32 msgnr, quint8 reply, QVariant answer) {
        Q_UNUSED(msgnr)
        m_lastReply = reply;
        m_lastAnswer = answer;
    });
}
