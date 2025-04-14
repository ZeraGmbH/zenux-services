#ifndef MOCKDEVICENODESEC_H
#define MOCKDEVICENODESEC_H

#include <abstractdevicenodesec.h>
#include <QJsonArray>
#include <QJsonObject>

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
private:
    QJsonArray m_dumpedIos;
};

#endif // MOCKDEVICENODESEC_H
