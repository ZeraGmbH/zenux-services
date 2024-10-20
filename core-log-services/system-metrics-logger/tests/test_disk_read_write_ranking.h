#ifndef TEST_DISK_READ_WRITE_RANKING_H
#define TEST_DISK_READ_WRITE_RANKING_H

#include <QObject>

class test_disk_read_write_ranking : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void twoNonZeroIosCreateTwoEntries();
    void zeroIosCreateNoEntries();
    void validProcessNames();
    void goneProcessNamesDontCreateEntry();
    void validByteNums();
    void validTotalNums();
    void goneProcessNotInTotalNums();
    void checkSort();
    void checkSortIdenticalIos();
};

#endif // TEST_DISK_READ_WRITE_RANKING_H
