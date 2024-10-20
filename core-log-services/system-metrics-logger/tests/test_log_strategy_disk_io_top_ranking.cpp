#include "test_log_strategy_disk_io_top_ranking.h"
#include "logstrategydiskiotopranking.h"
#include <QTest>

QTEST_MAIN(test_log_strategy_disk_io_top_ranking)

void test_log_strategy_disk_io_top_ranking::oneProcessWithValuesCreates100PercentReadWriteLines()
{
    QStringList logOutput;
    LogStrategyDiskIoTopRanking strat(2, [&](QString log) {
        logOutput.append(log);
    });
    DiskValuesProcesses processValues;
    processValues[1] = { 1, 2 };
    strat.addValue(processValues);
    QCOMPARE(logOutput.size(), 2);
    QCOMPARE(logOutput[0], "Read ranking: foo(100.0%)");
    QCOMPARE(logOutput[1], "Write ranking: foo(100.0%)");
}

/*void test_log_strategy_disk_io_top_ranking::twoProcessesTwoRankingsProduceFourLines()
{
    QStringList logOutput;
    LogStrategyDiskIoTopRanking strat(2, [&](QString log) {
        logOutput.append(log);
    });
    DiskValuesProcesses processValues;
    processValues[1] = { 1, 2 };
    processValues[2] = { 3, 6 };
    strat.addValue(processValues);
    QCOMPARE(logOutput.size(), 4);
}

void test_log_strategy_disk_io_top_ranking::oneProcesseTwoRankingsProduceTwoLines()
{
    QStringList logOutput;
    LogStrategyDiskIoTopRanking strat(2, [&](QString log) {
        logOutput.append(log);
    });
    DiskValuesProcesses processValues;
    processValues[1] = { 1, 2 };
    strat.addValue(processValues);
    QCOMPARE(logOutput.size(), 2);
}

void test_log_strategy_disk_io_top_ranking::twoProcessesOneRankingsProduceTwoLines()
{
    QStringList logOutput;
    LogStrategyDiskIoTopRanking strat(1, [&](QString log) {
        logOutput.append(log);
    });
    DiskValuesProcesses processValues;
    processValues[1] = { 1, 2 };
    processValues[2] = { 3, 6 };
    strat.addValue(processValues);
    QCOMPARE(logOutput.size(), 2);
}*/

