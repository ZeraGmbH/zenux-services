#ifndef TEST_LOG_STRATEGY_DISK_IO_TOP_RANKING_H
#define TEST_LOG_STRATEGY_DISK_IO_TOP_RANKING_H

#include <QObject>

class test_log_strategy_disk_io_top_ranking : public QObject
{
    Q_OBJECT
private slots:
    void multipleProcessesSameLoad();
    void limitRankingDepth();
    void multipleProcessesDifferentLoads();
    void noLogNoRead();
    void noLogNoWrite();
};

#endif // TEST_LOG_STRATEGY_DISK_IO_TOP_RANKING_H
