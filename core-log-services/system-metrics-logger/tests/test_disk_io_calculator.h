#ifndef TEST_DISK_IO_CALCULATOR_H
#define TEST_DISK_IO_CALCULATOR_H

#include <QObject>

class test_disk_io_calculator : public QObject
{
    Q_OBJECT
private slots:
    void zeroValuesInvalidDevice();
    void zeroValuesOnInvalidProcFile();
    void sequenceDisk1();
    void sequenceDisk2();
};

#endif // TEST_DISK_IO_CALCULATOR_H
