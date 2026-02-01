#ifndef MOCKDEVICENODEPCBMSG_H
#define MOCKDEVICENODEPCBMSG_H

#include <abstractdevicenodepcbmsg.h>

class MockDeviceNodePcbMsg : public AbstractDeviceNodePcbMsg
{
public:
    int open(const QString &devNodeFileName) override;
    void close() override;
    void enableFasync() override;
};

#endif // MOCKDEVICENODEPCBMSG_H
