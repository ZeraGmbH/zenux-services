#ifndef SECDEVICENODEMOCK_H
#define SECDEVICENODEMOCK_H

#include <secdevicenodeinterface.h>

class SecDeviceNodeMock : public SecDeviceNodeInterface
{
public:
    int open(QString devNodeFileName) override;
    void close() override;

    int lseek(ulong adr) override;
    int write(const char* buf, int len) override;
    int read(char* buf, int len) override;
    void enableFasync() override;
};

#endif // SECDEVICENODEMOCK_H
