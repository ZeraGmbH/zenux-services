#ifndef TEST_ADJDATADECODER_H
#define TEST_ADJDATADECODER_H

#include <QObject>

class test_adj_data_decoder : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void denyMaxSizeExceed();

    void readServerVersionAndDeviceNameForMT();

    void readServerVersionAndDeviceNameForCOM();
    void checkChannelRangeAvailability();
private:
    quint32 m_flashSizeAllDevicesAtTheTimeOfWriting;
};

#endif // TEST_ADJDATADECODER_H
