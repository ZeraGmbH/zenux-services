#include "test_source_control.h"
#include "jsonstructapi.h"
#include "proxy.h"
#include "proxyclient.h"
#include "zscpi_response_definitions.h"
#include "testfactoryi2cctrl.h"
#include <abstracttcpnetworkfactory.h>
#include <mocktcpnetworkfactory.h>
#include <timemachineobject.h>
#include <zera-json-params-state.h>
#include <testloghelpers.h>
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
    JsonStructApi structApi(capabilities);
    QCOMPARE(structApi.getDeviceName(), "MT581s2");

    QString expected = TestLogHelpers::loadFile(":/source_capabilities_valid_mt581s2.json");
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(expected, m_lastAnswer.toString()));
}

void test_source_control::mt310s2InitialState()
{
    setupServerAndClient("mt310s2d");
    m_pcbInterface->scpiCommand("UISRC:STATE?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(m_lastReply, ZSCPI::nak);
    QCOMPARE(m_lastAnswer, ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_source_control::mt581s2InitialState()
{
    setupServerAndClient("mt581s2d");

    m_pcbInterface->scpiCommand("UISRC:STATE?");
    TimeMachineObject::feedEventLoop();

    QString expected = TestLogHelpers::loadFile(":/source_state_valid_mt581s2.json");
    QCOMPARE(m_lastReply, ZSCPI::ack);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(expected, m_lastAnswer.toString()));
}

void test_source_control::mt310s2InitialLoad()
{
    setupServerAndClient("mt310s2d");
    m_pcbInterface->scpiCommand("UISRC:LOAD?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(m_lastReply, ZSCPI::nak);
    QCOMPARE(m_lastAnswer, ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_source_control::mt581s2InitialLoad()
{
    setupServerAndClient("mt581s2d");

    m_pcbInterface->scpiCommand("UISRC:CAPABILITIES?");
    TimeMachineObject::feedEventLoop();
    QJsonObject capabilities = QJsonDocument::fromJson(m_lastAnswer.toString().toUtf8()).object();
    ZeraJsonParamsState paramState(capabilities);
    QString expected = TestLogHelpers::dump(paramState.getDefaultJsonState());

    m_pcbInterface->scpiCommand("UISRC:LOAD?");
    TimeMachineObject::feedEventLoop();

    QCOMPARE(m_lastReply, ZSCPI::ack);
    QVERIFY(TestLogHelpers::compareAndLogOnDiffJson(expected, m_lastAnswer.toString()));
}

void test_source_control::mt310s2RejectValidCapabilitesSet()
{
    setupServerAndClient("mt310s2d");

    QString consideredValid = TestLogHelpers::loadFile(":/source_capabilities_valid_mt310s2.json");

    m_pcbInterface->scpiCommand(QString("UISRC:CAPABILITIES %1").arg(consideredValid));
    TimeMachineObject::feedEventLoop();

    QCOMPARE(m_lastReply, ZSCPI::nak);
    QCOMPARE(m_lastAnswer, ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_source_control::mt581s2RejectValidCapabilitesSet()
{
    setupServerAndClient("mt581s2d");

    QString consideredValid = TestLogHelpers::loadFile(":/source_capabilities_valid_mt581s2.json");

    m_pcbInterface->scpiCommand(QString("UISRC:CAPABILITIES %1").arg(consideredValid));
    TimeMachineObject::feedEventLoop();

    QCOMPARE(m_lastReply, ZSCPI::nak);
    QCOMPARE(m_lastAnswer, ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_source_control::mt310s2RejectValidStateSet()
{
    setupServerAndClient("mt310s2d");

    QString consideredValid = TestLogHelpers::loadFile(":/source_state_valid_mt310s2.json");

    m_pcbInterface->scpiCommand(QString("UISRC:STATE %1").arg(consideredValid));
    TimeMachineObject::feedEventLoop();

    QCOMPARE(m_lastReply, ZSCPI::nak);
    QCOMPARE(m_lastAnswer, ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_source_control::mt581s2RejectValidStateSet()
{
    setupServerAndClient("mt581s2d");

    QString consideredValidInOtherTest = TestLogHelpers::loadFile(":/source_state_valid_mt581s2.json");

    m_pcbInterface->scpiCommand(QString("UISRC:STATE %1").arg(consideredValidInOtherTest));
    TimeMachineObject::feedEventLoop();

    QCOMPARE(m_lastReply, ZSCPI::nak);
    QCOMPARE(m_lastAnswer, ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_source_control::mt310s2RejectInValidLoadSet()
{
    setupServerAndClient("mt310s2d");
    QString invalid = "UISRC:LOAD {\"foo\": \"bar\"}";
    m_pcbInterface->scpiCommand(invalid);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(m_lastReply, ZSCPI::nak);
    QCOMPARE(m_lastAnswer, ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_source_control::mt581s2RejectInValidLoadSet()
{
    setupServerAndClient("mt581s2d");
    QString invalid = "UISRC:LOAD {\"foo\": \"bar\"}";
    m_pcbInterface->scpiCommand(invalid);
    TimeMachineObject::feedEventLoop();

    QCOMPARE(m_lastReply, ZSCPI::nak);
    QCOMPARE(m_lastAnswer, ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_source_control::mt581s2AcceptValidLoadSetNoChange()
{
    setupServerAndClient("mt581s2d");

    m_pcbInterface->scpiCommand("UISRC:LOAD?");
    TimeMachineObject::feedEventLoop();

    m_pcbInterface->scpiCommand(QString("UISRC:LOAD %1").arg(m_lastAnswer.toString()));
    TimeMachineObject::feedEventLoop();

    QCOMPARE(m_lastReply, ZSCPI::ack);
    QCOMPARE(m_lastAnswer, ZSCPI::scpiAnswer[ZSCPI::ack]);
}

void test_source_control::setupServerAndClient(const QString &deviceD)
{
    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    m_mtXXXs2d = std::make_unique<MockMt310s2d>(std::make_shared<TestFactoryI2cCtrl>(true), tcpNetworkFactory, deviceD);
    TimeMachineObject::feedEventLoop();

    Zera::ProxyClientPtr proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307, tcpNetworkFactory);
    m_pcbInterface = std::make_unique<TinyZScpiCmdInterface>(proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(proxyClient);
    TimeMachineObject::feedEventLoop();

    connect(m_pcbInterface.get(), &TinyZScpiCmdInterface::serverAnswer, this, [&](quint32 msgnr, quint8 reply, const QVariant &answer) {
        Q_UNUSED(msgnr)
        m_lastReply = reply;
        m_lastAnswer = answer;
    });
}
