#ifndef MOCKDEVICENODERECORDER_H
#define MOCKDEVICENODERECORDER_H

#include <QString>
#include <QJsonArray>
#include <QJsonObject>

class MockDeviceNodeRecorder
{
public:
    void open(const QString &devNodeFileName);
    void close();

    void lseek(ulong adr);
    void write(const char* buf, int len);
    void read(char* buf, int len);
    void enableFasync();

    QJsonObject getRecording();
    void resetRecording();
private:
    QJsonArray m_dumpedIos;
};

#endif // MOCKDEVICENODERECORDER_H
