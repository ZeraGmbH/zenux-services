#ifndef TEST_DISK_IO_FOR_ALL_PROCESSES_H
#define TEST_DISK_IO_FOR_ALL_PROCESSES_H

#include <QObject>

class test_disk_io_for_all_processes : public QObject
{
    Q_OBJECT
private slots:
    void canRead();
    void firstReadNoValues();
    void secondReadDiffValues();
    void thirdReadValuesDown();
    void secondRemovesPid1ReadAgain();
    void secondAddsPid1ReadAgain();
};

#endif // TEST_DISK_IO_FOR_ALL_PROCESSES_H
