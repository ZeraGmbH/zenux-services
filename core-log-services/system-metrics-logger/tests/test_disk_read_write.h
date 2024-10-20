#ifndef TEST_DISK_READ_WRITE_H
#define TEST_DISK_READ_WRITE_H

#include <QObject>

class test_disk_read_write : public QObject
{
    Q_OBJECT
private slots:
    void allDiskBlockDevices();
    void allDiskBlockDevicesInvalidProcFile();
    void diskBlockDevicesOfInterest();
    void decodeLine();
    void decodeInvalidLines();
    void decodeInvalidProcFile();
    void decodeDevices();
};

#endif // TEST_DISK_READ_WRITE_H
