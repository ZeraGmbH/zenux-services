#ifndef MOCKDEVICENODESEC_H
#define MOCKDEVICENODESEC_H

#include <abstractdevicenodesec.h>

class MockDeviceNodeSec : public AbstractDeviceNodeSec
{
public:
    int open(QString devNodeFileName) override;
    void close() override;

    int lseek(ulong adr) override;
    int write(const char* buf, int len) override;
    int read(char* buf, int len) override;
    void enableFasync() override;
};

#endif // MOCKDEVICENODESEC_H
