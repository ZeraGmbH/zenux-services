#ifndef DEVICENODESEC_H
#define DEVICENODESEC_H

#include "abstractdevicenodesec.h"

class DeviceNodeSec : public AbstractDeviceNodeSec
{
public:
    int open(QString devNodeFileName) override;
    void close() override;

    int lseek(ulong adr) override;
    int write(const char* buf, int len) override;
    int read(char* buf, int len) override;
    void enableFasync() override;

private:
    int m_devFileDescriptor = -1;
    QString m_devNodeFileName;
};

#endif // DEVICENODESEC_H
