#include "test_adj_data_decoder.h"
#include "adjustmentdecoderinternal.h"
#include "adjustmenteepromreadwrite.h"
#include <QFile>
#include <QTest>

QTEST_MAIN(test_adj_data_decoder)

void test_adj_data_decoder::initTestCase()
{
    AdjustmentEepromReadWrite eepromRw("", 0, nullptr);
    m_flashSizeAllDevicesAtTheTimeOfWriting = eepromRw.getMaxSize();
}

void test_adj_data_decoder::denyMaxSizeExceed()
{
    QFile file(":/export_internal_initial_mt310s2.eeprom");
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();

    AdjustmentDecoderInternal reader1(ba.size());
    QVERIFY(reader1.decodeAdjBytes(ba));

    AdjustmentDecoderInternal reader2(ba.size()-1);
    QVERIFY(!reader2.decodeAdjBytes(ba));
}

void test_adj_data_decoder::readServerVersionAndDeviceNameForMT()
{
    QFile file(":/export_internal_initial_mt310s2.eeprom");
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();

    AdjustmentDecoderInternal reader(m_flashSizeAllDevicesAtTheTimeOfWriting);
    QVERIFY(reader.decodeAdjBytes(ba));
    std::shared_ptr<AdjustmentDataSet> adjData = reader.getAdjData();

    QCOMPARE(adjData->getAdjHeader().m_deviceName, "Unknown");
    QCOMPARE(adjData->getAdjHeader().m_serverVersion, "V1.01");
    QCOMPARE(adjData->getAdjHeader().m_serialNumber, "Unknown");
    QCOMPARE(adjData->getAdjHeader().m_deviceVersion, "DEVICE: Unknown;PCB: Unknown;LCA: Unknown;CTRL: Unknown");
    QCOMPARE(adjData->getAdjHeader().m_adjustmentDate, QDateTime::fromSecsSinceEpoch(0, Qt::UTC));
}

void test_adj_data_decoder::readMT310s2Ranges()
{
    QFile file(":/mt310s2-050059467.eeprom");
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();

    AdjustmentDecoderInternal reader(m_flashSizeAllDevicesAtTheTimeOfWriting);
    reader.decodeAdjBytes(ba);
    std::shared_ptr<AdjustmentDataSet> adjData = reader.getAdjData();

    QMap<QString, QStringList> rangesInfos = adjData->getRangeInfos();
    QCOMPARE(rangesInfos.size(), 8);

    QStringList expectedChannels = {"m0", "m1", "m2", "m3", "m4", "m5", "m6", "m7"};
    QCOMPARE(rangesInfos.keys(), expectedChannels);

    QStringList expectedRangesV = (QStringList() << "250V" << "8V" << "100mV");
    expectedRangesV.sort();
    QCOMPARE(rangesInfos.value("m0"), expectedRangesV);
    QCOMPARE(rangesInfos.value("m1"), expectedRangesV);
    QCOMPARE(rangesInfos.value("m2"), expectedRangesV);
    QCOMPARE(rangesInfos.value("m6"), expectedRangesV);

    QStringList expectedRangesI = QStringList() << "10A" << "5A" << "2.5A" << "1.0A" << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
    QStringList expectedInternalRangesI = QStringList() << "8V" << "5V" << "2V" << "1V" << "500mV" << "200mV" << "100mV" << "50mV" << "20mV" << "10mV" << "5mV" << "2mV";
    QStringList expectedTotalRangesI = expectedRangesI + expectedInternalRangesI;
    expectedTotalRangesI.sort();
    QCOMPARE(rangesInfos.value("m3"), expectedTotalRangesI);
    QCOMPARE(rangesInfos.value("m4"), expectedTotalRangesI);
    QCOMPARE(rangesInfos.value("m5"), expectedTotalRangesI);
    QStringList expectedRangesIAUX = (QStringList() << "0A") + expectedInternalRangesI;
    expectedRangesIAUX.sort();
    QCOMPARE(rangesInfos.value("m7"), expectedRangesIAUX);
}

void test_adj_data_decoder::readServerVersionAndDeviceNameForCOM()
{
    QFile file(":/export_internal_initial_com5003.eeprom");
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();

    AdjustmentDecoderInternal reader(m_flashSizeAllDevicesAtTheTimeOfWriting);
    QVERIFY(reader.decodeAdjBytes(ba));
    std::shared_ptr<AdjustmentDataSet> adjData = reader.getAdjData();

    QCOMPARE(adjData->getAdjHeader().m_deviceName, "Unknown");
    QCOMPARE(adjData->getAdjHeader().m_serverVersion, "V1.00");
    QCOMPARE(adjData->getAdjHeader().m_serialNumber, "Unknown");
    QCOMPARE(adjData->getAdjHeader().m_deviceVersion, "DEVICE: Unknown;PCB: Unknown;LCA: Unknown;CTRL: Unknown");
    QCOMPARE(adjData->getAdjHeader().m_adjustmentDate, QDateTime::fromSecsSinceEpoch(0, Qt::UTC));
}

void test_adj_data_decoder::readCOM5003Ranges()
{
    QFile file(":/export_internal_initial_com5003.eeprom");
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();

    AdjustmentDecoderInternal reader(m_flashSizeAllDevicesAtTheTimeOfWriting);
    reader.decodeAdjBytes(ba);
    std::shared_ptr<AdjustmentDataSet> adjData = reader.getAdjData();

    QMap<QString, QStringList> rangesInfos = adjData->getRangeInfos();
    QCOMPARE(rangesInfos.size(), 6);

    QStringList expectedChannels = {"m0", "m1", "m2", "m3", "m4", "m5"};
    QCOMPARE(rangesInfos.keys(), expectedChannels);

    QStringList expectedRangesV = QStringList() << "480V" << "240V" << "120V" << "60V" << "12V" << "5V";
    QStringList expectedInternalRanges = QStringList() << "R0V" << "R10V";
    QStringList expectedTotalRangesV = expectedRangesV + expectedInternalRanges;
    expectedTotalRangesV.sort();

    QCOMPARE(rangesInfos.value("m0"), expectedTotalRangesV);
    QCOMPARE(rangesInfos.value("m1"), expectedTotalRangesV);
    QCOMPARE(rangesInfos.value("m2"), expectedTotalRangesV);

    QStringList expectedRangesI = QStringList() << "200A" << "100A" << "50A" << "25A" << "10A" << "5A" << "2.5A" << "1.0A" <<"500mA" << "250mA" << "100mA" << "50mA" << "25mA" << "10mA" << "5mA";
    QStringList expectedTotalRangesI = expectedRangesI + expectedInternalRanges;
    expectedTotalRangesI.sort();
    QCOMPARE(rangesInfos.value("m3"), expectedTotalRangesI);
    QCOMPARE(rangesInfos.value("m4"), expectedTotalRangesI);
    QCOMPARE(rangesInfos.value("m5"), expectedTotalRangesI);
}

void test_adj_data_decoder::checkChannelRangeAvailability()
{
    QFile file(":/export_internal_initial_com5003.eeprom");
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();

    AdjustmentDecoderInternal reader(m_flashSizeAllDevicesAtTheTimeOfWriting);
    reader.decodeAdjBytes(ba);
    std::shared_ptr<AdjustmentDataSet> adjData = reader.getAdjData();

    QVERIFY(adjData->isChannelRangeAvailable("m0", "480V"));
    QVERIFY(!adjData->isChannelRangeAvailable("m0", "48V"));
    QVERIFY(!adjData->isChannelRangeAvailable("m10", "480V"));
}
