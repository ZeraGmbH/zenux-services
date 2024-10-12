#ifndef TEST_DISK_IO_TOTAL_CALCULATOR_H
#define TEST_DISK_IO_TOTAL_CALCULATOR_H

#include <QObject>

class test_disk_io_total_calculator : public QObject
{
    Q_OBJECT
private slots:
    void zeroValuesOnNoDevices();
    void sequenceDisk1Disk2();
};

#endif // TEST_DISK_IO_TOTAL_CALCULATOR_H
