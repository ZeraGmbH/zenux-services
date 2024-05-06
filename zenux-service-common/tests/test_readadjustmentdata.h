#ifndef TEST_READADJUSTEDDATA_H
#define TEST_READADJUSTEDDATA_H

#include <QObject>

class test_readadjustmentdata : public QObject
{
    Q_OBJECT
private slots:
    void checkFileNotEmpty();
    void readServerVersionAndDeviceNameForMT();
    void readMT310s2Ranges();

    void readServerVersionAndDeviceNameForCOM();
    void readCOM5003Ranges();
    void checkChannelRangeAvailability();
};

#endif // TEST_READADJUSTEDDATA_H
