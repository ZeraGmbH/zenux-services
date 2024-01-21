#ifndef MOCKPCBDEVICENODEMESSAGE_H
#define MOCKPCBDEVICENODEMESSAGE_H

#include <abstractpcbmsgdevicenode.h>

class MockPcbDeviceNodeMessage : public AbstractPcbMsgDeviceNode
{
public:
    int open(QString devNodeFileName) override;
    void close() override;
    void enableFasync() override;
};

#endif // MOCKPCBDEVICENODEMESSAGE_H
