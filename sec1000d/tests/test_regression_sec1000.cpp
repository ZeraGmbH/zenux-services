#include "test_regression_sec1000.h"
#include "mockdevicenodesec.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include "mocksingletondevicenodesec.h"
#include "zscpi_response_definitions.h"
#include <xmldocumentcompare.h>
#include <testloghelpers.h>
#include <timemachineobject.h>
#include <mocktcpnetworkfactory.h>
#include <QTest>
#include <unistd.h>
#include <signal.h>

QTEST_MAIN(test_regression_sec1000);

void test_regression_sec1000::initTestCase_data()
{
    QTest::addColumn<int>("ecUnitCount");
    QTest::addColumn<QString>("deviceFamily");
    QTest::newRow("COM5003") << cSEC1000dServer::Com5003EcUnitCount << "com5003";
    QTest::newRow("MTxxxs2") << cSEC1000dServer::Mtxxxs2EcUnitCount << "mtxxxs2";
}

void test_regression_sec1000::init()
{
    AbstractDeviceNodeSecPtr deviceNode = MockSingletonDeviceNodeSec::getInstancePtr();
    MockDeviceNodeSec *secDeviceNode = static_cast<MockDeviceNodeSec*>(deviceNode.get());
    secDeviceNode->resetRecording();

    VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory = VeinTcp::MockTcpNetworkFactory::create();
    m_resman = std::make_unique<ResmanRunFacade>(tcpNetworkFactory);
    QFETCH_GLOBAL(int, ecUnitCount);
    m_server = std::make_unique<MockSec1000d>(tcpNetworkFactory, ecUnitCount);
    TimeMachineObject::feedEventLoop();

    m_proxyClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6305, tcpNetworkFactory);
    m_secIFace = std::make_unique<Zera::cSECInterface>();
    m_secIFace->setClientSmart(m_proxyClient);
    Zera::Proxy::getInstance()->startConnectionSmart(m_proxyClient);
    TimeMachineObject::feedEventLoop();
}

void test_regression_sec1000::cleanup()
{
    m_secIFace = nullptr;
    m_proxyClient = nullptr;
    m_server = nullptr;
    m_resman = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_sec1000::serverUp()
{
    QString ret = ScpiSingleTransactionBlocked::query("SYSTEM:VERSION:SERVER?", 6305);
    QCOMPARE(ret, "V42.0");
}

void test_regression_sec1000::dumpScpi()
{
    QFETCH_GLOBAL(QString, deviceFamily);
    QString expected = TestLogHelpers::loadFile(QString(":/scpi/scpi-dump-%1.xml").arg(deviceFamily));

    QString dumped = ScpiSingleTransactionBlocked::query("SYSTEM:INTERFACE:READ?", 6305);
    XmlDocumentCompare compare;
    bool ok = compare.compareXml(dumped, expected);
    if(!ok)
        TestLogHelpers::compareAndLogOnDiff(expected, dumped);
    QVERIFY(ok);
}

void test_regression_sec1000::interruptDeviceNodeIo()
{
    AbstractDeviceNodeSecPtr deviceNode = MockSingletonDeviceNodeSec::getInstancePtr();
    MockDeviceNodeSec *secDeviceNode = static_cast<MockDeviceNodeSec*>(deviceNode.get());
    secDeviceNode->resetRecording();

    fireInterrupt();

    QFETCH_GLOBAL(QString, deviceFamily);
    QString expected = TestLogHelpers::loadFile(QString(":/devnode/dump-interrupt-%1.json").arg(deviceFamily));
    QByteArray dumped = TestLogHelpers::dump(secDeviceNode->getRecording());
    QVERIFY(TestLogHelpers::compareAndLogOnDiff(expected, dumped));
}

void test_regression_sec1000::interruptNotifications()
{
    QFETCH_GLOBAL(int, ecUnitCount);
    QStringList ecalChannels = claimAllEcChannels(ecUnitCount);

    QList<ServerSendData> answersOnRegister;
    collectServerSends(answersOnRegister);
    registerInterruptNotifiers(ecalChannels);
    QCOMPARE(answersOnRegister.count(), ecalChannels.count());
    QVERIFY(checkRegisterResponses(answersOnRegister));

    prepareInterruptMaskValue(ecalChannels);
    QList<ServerSendData> serverNotifications;
    collectServerSends(serverNotifications);
    fireInterrupt();
    QCOMPARE(serverNotifications.count(), ecalChannels.count());
    QVERIFY(checkInterruptMasks(serverNotifications));
}

void test_regression_sec1000::fireInterrupt()
{
    kill(getpid(), SIGIO);
    TimeMachineObject::feedEventLoop();
}

QStringList test_regression_sec1000::claimAllEcChannels(int ecUnitCount)
{
    QString scpiClaimEcChannels = QString("ECAL:SET");
    QString ecalChannelsStr = ScpiSingleTransactionBlocked::cmd(scpiClaimEcChannels, QString("%1").arg(ecUnitCount), 6305);
    QStringList ecalChannels = ecalChannelsStr.split(";", Qt::SkipEmptyParts);
    return ecalChannels;
}

void test_regression_sec1000::collectServerSends(QList<ServerSendData> &serverSendList)
{
    connect(m_secIFace.get(), &AbstractServerInterface::serverAnswer, this, [&](quint32 msgnr, quint8 reply, QVariant answer) {
        serverSendList.append({msgnr, reply, answer});
    });
}

void test_regression_sec1000::registerInterruptNotifiers(QStringList ecalChannels)
{
    for(const QString &ecChannel : ecalChannels) {
        // This assembly language was stolen from secmodule (and friends)...
        QString scpi = QString("ECAL:%1:R%2?").arg(ecChannel).arg(ECALCREG::INTREG);
        m_secIFace->registerNotifier(scpi);
    }
    TimeMachineObject::feedEventLoop();
}

bool test_regression_sec1000::checkRegisterResponses(const QList<ServerSendData> &responses)
{
    for (const ServerSendData &response : responses)
        if (response.answer != ZSCPI::scpiAnswer[ZSCPI::ack])
            return false;
    return true;
}

void test_regression_sec1000::prepareInterruptMaskValue(QStringList ecalChannels)
{
    QByteArray interruptResponseValues;
    // Wow: In NotificationValue::setValue we learn that notifications are spawned on
    // rising edge of interrupt flags only (makes sense but is hidden to dump readers like
    // me). So hack some rising edges here...
    for(int ecChannelNum = 0; ecChannelNum<ecalChannels.count(); ecChannelNum++) {
        quint8 halfByteMaskLow = calcNonZeroTestMask(ecChannelNum);
        if(ecChannelNum % 2 == 0)
            interruptResponseValues.append(halfByteMaskLow);
        else {
            quint8 mask = interruptResponseValues[interruptResponseValues.size()-1];
            mask |= (halfByteMaskLow << 4);
            interruptResponseValues[interruptResponseValues.size()-1] = mask;
        }
    }
    AbstractDeviceNodeSecPtr deviceNode = MockSingletonDeviceNodeSec::getInstancePtr();
    MockDeviceNodeSec *secDeviceNode = static_cast<MockDeviceNodeSec*>(deviceNode.get());
    secDeviceNode->setNextReadValue(interruptResponseValues);
}

bool test_regression_sec1000::checkInterruptMasks(const QList<ServerSendData> &responses)
{
    for (int i=0; i<responses.count(); i++) {
        // Yes this is a regression test assuming all interrupts send in channel order
        QString expected = QString("IRQ:%1").arg(calcNonZeroTestMask(i));
        QString found = responses[i].answer.toString();
        if (expected != found) {
            qWarning("Interrupt %i has wrong answer: expected: %s / found: %s",
                     i, qPrintable(expected), qPrintable(found));
            return false;
        }
    }
    return true;
}

quint8 test_regression_sec1000::calcNonZeroTestMask(int ecChannelNum)
{
    quint8 mask = ecChannelNum & 0x0F;
    if(mask == 0)
        mask = 0x0F;
    return mask;
}

