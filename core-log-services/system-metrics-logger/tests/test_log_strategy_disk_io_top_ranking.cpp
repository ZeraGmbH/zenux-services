#include "test_log_strategy_disk_io_top_ranking.h"
#include "logstrategydiskiotopranking.h"
#include "testsysteminfofilelocator.h"
#include <QTest>

QTEST_MAIN(test_log_strategy_disk_io_top_ranking)

void test_log_strategy_disk_io_top_ranking::multipleProcessesSameLoad()
{
    TestSystemInfoFileLocator::setProcBasePath(":/proc_pid_io/default");
    QStringList logOutput;
    LogStrategyDiskIoTopRanking strat(3, [&](QString log) {
        logOutput.append(log);
    });
    DiskValuesProcesses processValues;
    processValues[1] = { 1024, 2*1024 };
    processValues[2] = { 1024, 2*1024 };
    processValues[3] = { 1024, 2*1024 };
    strat.addValue(processValues);
    QCOMPARE(logOutput.size(), 2);
    QCOMPARE(logOutput[0], "Read ranking: foo(1.0KiB / 33.3%) | bar(1.0KiB / 33.3%) | baz(1.0KiB / 33.3%)");
    QCOMPARE(logOutput[1], "Write ranking: foo(2.0KiB / 33.3%) | bar(2.0KiB / 33.3%) | baz(2.0KiB / 33.3%)");
}

void test_log_strategy_disk_io_top_ranking::limitRankingDepth()
{
    TestSystemInfoFileLocator::setProcBasePath(":/proc_pid_io/default");
    QStringList logOutput;
    LogStrategyDiskIoTopRanking strat(2, [&](QString log) {
        logOutput.append(log);
    });
    DiskValuesProcesses processValues;
    processValues[1] = { 1024, 2*1024 };
    processValues[2] = { 1024, 2*1024 };
    processValues[3] = { 1024, 2*1024 };
    strat.addValue(processValues);
    QCOMPARE(logOutput.size(), 2);
    QCOMPARE(logOutput[0], "Read ranking: foo(1.0KiB / 33.3%) | bar(1.0KiB / 33.3%)");
    QCOMPARE(logOutput[1], "Write ranking: foo(2.0KiB / 33.3%) | bar(2.0KiB / 33.3%)");
}

void test_log_strategy_disk_io_top_ranking::multipleProcessesDifferentLoads()
{
    TestSystemInfoFileLocator::setProcBasePath(":/proc_pid_io/default");
    QStringList logOutput;
    LogStrategyDiskIoTopRanking strat(3, [&](QString log) {
        logOutput.append(log);
    });
    DiskValuesProcesses processValues;
    processValues[1] = { 1024, 3*1024 };
    processValues[2] = { 3*1024, 1024 };
    strat.addValue(processValues);
    QCOMPARE(logOutput.size(), 2);
    QCOMPARE(logOutput[0], "Read ranking: bar(3.0KiB / 75.0%) | foo(1.0KiB / 25.0%)");
    QCOMPARE(logOutput[1], "Write ranking: foo(3.0KiB / 75.0%) | bar(1.0KiB / 25.0%)");
}

void test_log_strategy_disk_io_top_ranking::noLogNoRead()
{
    TestSystemInfoFileLocator::setProcBasePath(":/proc_pid_io/default");
    QStringList logOutput;
    LogStrategyDiskIoTopRanking strat(3, [&](QString log) {
        logOutput.append(log);
    });
    DiskValuesProcesses processValues;
    processValues[1] = { 0, 2*1024 };
    processValues[2] = { 0, 2*1024 };
    processValues[3] = { 0, 2*1024 };
    strat.addValue(processValues);
    QCOMPARE(logOutput.size(), 1);
    QCOMPARE(logOutput[0], "Write ranking: foo(2.0KiB / 33.3%) | bar(2.0KiB / 33.3%) | baz(2.0KiB / 33.3%)");
}

void test_log_strategy_disk_io_top_ranking::noLogNoWrite()
{
    TestSystemInfoFileLocator::setProcBasePath(":/proc_pid_io/default");
    QStringList logOutput;
    LogStrategyDiskIoTopRanking strat(3, [&](QString log) {
        logOutput.append(log);
    });
    DiskValuesProcesses processValues;
    processValues[1] = { 1024, 0 };
    processValues[2] = { 1024, 0 };
    processValues[3] = { 1024, 0 };
    strat.addValue(processValues);
    QCOMPARE(logOutput.size(), 1);
    QCOMPARE(logOutput[0], "Read ranking: foo(1.0KiB / 33.3%) | bar(1.0KiB / 33.3%) | baz(1.0KiB / 33.3%)");
}
