#include "test_regression_adj_status_mt310s2.h"
#include "adjustmentflags.h"
#include "proxy.h"
#include "scpisingletransactionblocked.h"
#include "mocki2ceepromiofactory.h"
#include "clampfactorytest.h"
#include "testfactoryi2cctrl.h"
#include "zscpi_response_definitions.h"
#include <timemachineobject.h>
#include <QTest>

QTEST_MAIN(test_regression_adj_status_mt310s2);

void test_regression_adj_status_mt310s2::initTestCase()
{
    ClampFactoryTest::enableTest();
    MockI2cEEpromIoFactory::enableMock();
}

void test_regression_adj_status_mt310s2::cleanup()
{
    m_pcbClient = nullptr;
    m_testServer = nullptr;
    m_resmanServer = nullptr;
    TimeMachineObject::feedEventLoop();
}

void test_regression_adj_status_mt310s2::statusDefaultMachine()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));

    QString ret = ScpiSingleTransactionBlocked::query("STATUS:PCB:ADJUSTMENT?");
    QCOMPARE(ret, QString::number(Adjustment::notAdjusted | Adjustment::wrongSNR));
}

void test_regression_adj_status_mt310s2::statusAllAdjusted()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    QVERIFY(setAdjStatusAllChannelRanges(true));

    QString ret = ScpiSingleTransactionBlocked::query("STATUS:PCB:ADJUSTMENT?");
    QCOMPARE(ret, QString::number(Adjustment::adjusted));
}

void test_regression_adj_status_mt310s2::statusClampsI1DefaultMachine()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));

    QString ret = ScpiSingleTransactionBlocked::query("STATUS:CLAMP:m3:ADJUSTMENT?");
    QCOMPARE(ret, ZSCPI::scpiAnswer[ZSCPI::nak]);
}

void test_regression_adj_status_mt310s2::statusClampsI1DefaultCL120A()
{
    setupServers(std::make_shared<TestFactoryI2cCtrl>(true));
    m_testServer->addClamp(CL120A, "IL1");

    QString ret = ScpiSingleTransactionBlocked::query("STATUS:CLAMP:m3:ADJUSTMENT?");
    QCOMPARE(ret, QString::number(Adjustment::notAdjusted));
}

void test_regression_adj_status_mt310s2::setupServers(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    PermissionFunctions::setPermissionCtrlFactory(ctrlFactory);

    m_resmanServer = std::make_unique<ResmanRunFacade>();
    m_testServer = std::make_unique<TestServerForSenseInterfaceMt310s2>(ctrlFactory);
    TimeMachineObject::feedEventLoop();

    m_pcbClient = Zera::Proxy::getInstance()->getConnectionSmart("127.0.0.1", 6307);
    Zera::Proxy::getInstance()->startConnectionSmart(m_pcbClient);
    TimeMachineObject::feedEventLoop();
}

bool test_regression_adj_status_mt310s2::setAdjStatusAllChannelRanges(bool adjusted)
{
    QString adjStatSendVal = adjusted ? "128" : 0;
    QString channels = ScpiSingleTransactionBlocked::query("SENSE:CHANNEL:CATALOG?");
    QStringList channelList = channels.split(";");
    bool allOk = channelList.count() >= 6;
    for(const auto &channel : channelList) {
        QString ranges = ScpiSingleTransactionBlocked::query(QString("SENSE:%1:RANGE:CATALOG?").arg(channel));
        QStringList rangeList = ranges.split(";");
        for(const auto &range : rangeList) {
            QString ret = ScpiSingleTransactionBlocked::cmd(QString("SENSE:%1:%2:CORR:GAIN:STAT").arg(channel, range), adjStatSendVal);
            if(ret != ZSCPI::scpiAnswer[ZSCPI::ack])
                allOk = false;
            ret = ScpiSingleTransactionBlocked::cmd(QString("SENSE:%1:%2:CORR:PHASE:STAT").arg(channel, range), adjStatSendVal);
            if(ret != ZSCPI::scpiAnswer[ZSCPI::ack])
                allOk = false;
            ret = ScpiSingleTransactionBlocked::cmd(QString("SENSE:%1:%2:CORR:OFFSET:STAT").arg(channel, range), adjStatSendVal);
            if(ret != ZSCPI::scpiAnswer[ZSCPI::ack])
                allOk = false;
        }
    }
    if(ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:WRITE", "") != ZSCPI::scpiAnswer[ZSCPI::ack])
        allOk = false;
    // TODO? is why is re-read necessary
    if(ScpiSingleTransactionBlocked::cmd("SYSTEM:ADJUSTMENT:FLASH:READ", "") != ZSCPI::scpiAnswer[ZSCPI::ack])
        allOk = false;
    return allOk;
}
