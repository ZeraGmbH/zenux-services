#include "test_criticalstatusmock.h"
#include "mocki2cctrlcriticalstatus.h"
#include <QTest>

QTEST_MAIN(test_criticalstatusmock);

void test_criticalstatusmock::resetCriticalStatus()
{
    quint16 criticalStatus = 0xFFFF;
    quint16 criticalStatusMask = 0xFFFF;
    MockI2cCtrlCriticalStatus mockCritStat(criticalStatus, criticalStatusMask);

    quint16 critStat = 0;
    mockCritStat.readCriticalStatus(critStat);
    QCOMPARE(critStat, 0xFFFF);

    mockCritStat.resetCriticalStatus(0x0001);
    mockCritStat.readCriticalStatus(critStat);
    QCOMPARE(critStat, 0xFFFE);

    mockCritStat.resetCriticalStatus(0x0001);
    mockCritStat.readCriticalStatus(critStat);
    QCOMPARE(critStat, 0xFFFE);

    mockCritStat.resetCriticalStatus(0x0002);
    mockCritStat.readCriticalStatus(critStat);
    QCOMPARE(critStat, 0xFFFC);
}
