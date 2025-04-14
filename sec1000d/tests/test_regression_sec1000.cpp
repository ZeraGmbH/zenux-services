#include "test_regression_sec1000.h"
#include "mockdevicenodesec.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include "mocksingletondevicenodesec.h"
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

    int answersOnRegister = 0;
    collectServerSends(answersOnRegister);
    registerInterruptNotifiers(ecalChannels);
    QCOMPARE(answersOnRegister, ecalChannels.count());

    prepareInterruptMaskValue(ecalChannels);
    int serverNotifications = 0;
    collectServerSends(serverNotifications);
    fireInterrupt();
    QCOMPARE(serverNotifications, ecalChannels.count());
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

void test_regression_sec1000::collectServerSends(int &serverSendCount)
{
    connect(m_secIFace.get(), &AbstractServerInterface::serverAnswer, this, [&](quint32 msgnr, quint8 reply, QVariant answer) {
        Q_UNUSED(msgnr)
        Q_UNUSED(reply)
        Q_UNUSED(answer)
        serverSendCount++;
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

void test_regression_sec1000::prepareInterruptMaskValue(QStringList ecalChannels)
{
    QByteArray interruptResponseValues;
    // Wow: In NotificationValue::setValue we learn that notifications are spawned on
    // rising edge of interrupt flags only (makes sense but is hidden to dump readers like
    // me). So hack some rising edges here...
    for(int ecChannelNum = 0; ecChannelNum<ecalChannels.count(); ecChannelNum++) {
        if(ecChannelNum % 2 == 0)
            interruptResponseValues.append(0x0F);
        else
            interruptResponseValues.back()= 0xFF;
    }
    AbstractDeviceNodeSecPtr deviceNode = MockSingletonDeviceNodeSec::getInstancePtr();
    MockDeviceNodeSec *secDeviceNode = static_cast<MockDeviceNodeSec*>(deviceNode.get());
    secDeviceNode->setNextReadValue(interruptResponseValues);
}

