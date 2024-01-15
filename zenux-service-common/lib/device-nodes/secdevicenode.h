#ifndef SECDEVICENODE_H
#define SECDEVICENODE_H

#include "secdevicenodeabstract.h"

class SecDeviceNode : public SecDeviceNodeAbstract
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

#endif // SECDEVICENODE_H
