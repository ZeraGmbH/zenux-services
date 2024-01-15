#ifndef MOCKSECDEVICENODE_H
#define MOCKSECDEVICENODE_H

#include <secdevicenodeabstract.h>

class MockSecDeviceNode : public SecDeviceNodeAbstract
{
public:
    int open(QString devNodeFileName) override;
    void close() override;

    int lseek(ulong adr) override;
    int write(const char* buf, int len) override;
    int read(char* buf, int len) override;
    void enableFasync() override;
};

#endif // MOCKSECDEVICENODE_H
