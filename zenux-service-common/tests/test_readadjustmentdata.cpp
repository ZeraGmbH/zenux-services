#include "test_readadjustmentdata.h"
#include "adjustmentdecoderinternal.h"
#include "adjustmenteepromreadwrite.h"
#include <QFile>
#include <QTest>

QTEST_MAIN(test_readadjustmentdata)

void test_readadjustmentdata::initTestCase()
{
    AdjustmentEepromReadWrite eepromRw("", 0, nullptr);
    m_flashSizeAllDevicesAtTheTimeOfWriting = eepromRw.getMaxSize();
}

void test_readadjustmentdata::denyMaxSizeExceed()
{
    QFile file(":/export_internal_initial_mt310s2.eeprom");
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();

    AdjustmentDecoderInternal reader1(ba.size());
    QVERIFY(reader1.decodeAdjBytes(ba));

    AdjustmentDecoderInternal reader2(ba.size()-1);
    QVERIFY(!reader2.decodeAdjBytes(ba));
}

void test_readadjustmentdata::readServerVersionAndDeviceNameForMT()
{
    QFile file(":/export_internal_initial_mt310s2.eeprom");
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();

    AdjustmentDecoderInternal reader(m_flashSizeAllDevicesAtTheTimeOfWriting);
    QVERIFY(reader.decodeAdjBytes(ba));
    QCOMPARE(reader.getAdjHeader().m_deviceName, "Unknown");
    QCOMPARE(reader.getAdjHeader().m_serverVersion, "V1.01");
    QCOMPARE(reader.getAdjHeader().m_serialNumber, "Unknown");
    QCOMPARE(reader.getAdjHeader().m_deviceVersion, "DEVICE: Unknown;PCB: Unknown;LCA: Unknown;CTRL: Unknown");
    QCOMPARE(reader.getAdjHeader().m_adjustmentDate, QDateTime::fromSecsSinceEpoch(0, Qt::UTC));
}

void test_readadjustmentdata::readMT310s2Ranges()
{
    QFile file(":/export_internal_initial_mt310s2.eeprom");
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();

    AdjustmentDecoderInternal reader(m_flashSizeAllDevicesAtTheTimeOfWriting);
    reader.decodeAdjBytes(ba);

    QMap<QString, QStringList> rangesInfos = reader.getRangeInfos();
    QCOMPARE(rangesInfos.size(), 8);

    QStringList expectedChannels = {"m0", "m1", "m2", "m3", "m4", "m5", "m6", "m7"};
    QCOMPARE(rangesInfos.keys(), expectedChannels);

    QStringList expectedRangesV = QStringList() << "250V" << "8V" << "100mV";
    QCOMPARE(rangesInfos.value("m0"), expectedRangesV);
    QCOMPARE(rangesInfos.value("m1"), expectedRangesV);
    QCOMPARE(rangesInfos.value("m2"), expectedRangesV);
    QCOMPARE(rangesInfos.value("m6"), expectedRangesV);

    QStringList expectedRangesI = QStringList() << "10A" << "5A" << "2.5A" << "1.0A" << "500mA" << "250mA" << "100mA" << "50mA" << "25mA";
    QStringList expectedInternalRangesI = QStringList() << "8V" << "5V" << "2V" << "1V" << "500mV" << "200mV" << "100mV" << "50mV" << "20mV" << "10mV" << "5mV" << "2mV";
    QCOMPARE(rangesInfos.value("m3"), expectedRangesI + expectedInternalRangesI);
    QCOMPARE(rangesInfos.value("m4"), expectedRangesI + expectedInternalRangesI);
    QCOMPARE(rangesInfos.value("m5"), expectedRangesI + expectedInternalRangesI);
    QStringList expectedRangesIAUX = (QStringList() << "0A") + expectedInternalRangesI;
    QCOMPARE(rangesInfos.value("m7"), expectedRangesIAUX);
}

void test_readadjustmentdata::readServerVersionAndDeviceNameForCOM()
{
    QFile file(":/export_internal_initial_com5003.eeprom");
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();

    AdjustmentDecoderInternal reader(m_flashSizeAllDevicesAtTheTimeOfWriting);
    QVERIFY(reader.decodeAdjBytes(ba));
    QCOMPARE(reader.getAdjHeader().m_deviceName, "Unknown");
    QCOMPARE(reader.getAdjHeader().m_serverVersion, "V1.00");
    QCOMPARE(reader.getAdjHeader().m_serialNumber, "Unknown");
    QCOMPARE(reader.getAdjHeader().m_deviceVersion, "DEVICE: Unknown;PCB: Unknown;LCA: Unknown;CTRL: Unknown");
    QCOMPARE(reader.getAdjHeader().m_adjustmentDate, QDateTime::fromSecsSinceEpoch(0, Qt::UTC));
}

void test_readadjustmentdata::readCOM5003Ranges()
{
    QFile file(":/export_internal_initial_com5003.eeprom");
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();

    AdjustmentDecoderInternal reader(m_flashSizeAllDevicesAtTheTimeOfWriting);
    reader.decodeAdjBytes(ba);

    QMap<QString, QStringList> rangesInfos = reader.getRangeInfos();
    QCOMPARE(rangesInfos.size(), 6);

    QStringList expectedChannels = {"m0", "m1", "m2", "m3", "m4", "m5"};
    QCOMPARE(rangesInfos.keys(), expectedChannels);

    QStringList expectedRangesV = QStringList() << "480V" << "240V" << "120V" << "60V" << "12V" << "5V";
    QStringList expectedInternalRanges = QStringList() << "R0V" << "R10V";
    QCOMPARE(rangesInfos.value("m0"), expectedRangesV + expectedInternalRanges);
    QCOMPARE(rangesInfos.value("m1"), expectedRangesV + expectedInternalRanges);
    QCOMPARE(rangesInfos.value("m2"), expectedRangesV + expectedInternalRanges);

    QStringList expectedRangesI = QStringList() << "200A" << "100A" << "50A" << "25A" << "10A" << "5A" << "2.5A" << "1.0A" <<"500mA" << "250mA" << "100mA" << "50mA" << "25mA" << "10mA" << "5mA";
    QCOMPARE(rangesInfos.value("m3"), expectedRangesI + expectedInternalRanges);
    QCOMPARE(rangesInfos.value("m4"), expectedRangesI + expectedInternalRanges);
    QCOMPARE(rangesInfos.value("m5"), expectedRangesI + expectedInternalRanges);
}

void test_readadjustmentdata::checkChannelRangeAvailability()
{
    QFile file(":/export_internal_initial_com5003.eeprom");
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();

    AdjustmentDecoderInternal reader(m_flashSizeAllDevicesAtTheTimeOfWriting);
    reader.decodeAdjBytes(ba);

    QVERIFY(reader.isChannelRangeAvailable("m0", "480V"));
    QVERIFY(!reader.isChannelRangeAvailable("m0", "48V"));
    QVERIFY(!reader.isChannelRangeAvailable("m10", "480V"));
}

