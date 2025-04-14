#ifndef MOCKDEVICENODESEC_H
#define MOCKDEVICENODESEC_H

#include <abstractdevicenodesec.h>
#include "mockdevicenoderecorder.h"

class MockDeviceNodeSec : public AbstractDeviceNodeSec
{
public:
    int open(QString devNodeFileName) override;
    void close() override;

    int lseek(ulong adr) override;
    int write(const char* buf, int len) override;
    int read(char* buf, int len) override;
    void enableFasync() override;

    QJsonObject getRecording();
    void resetRecording();
    void setNextReadValue(QByteArray nextReadValue); // Try simple first...
private:
    MockDeviceNodeRecorder m_deviceNodeRecorder;
    QByteArray m_nextReadValue;
};

#endif // MOCKDEVICENODESEC_H
