#ifndef DEVICENODEPCBMSG_H
#define DEVICENODEPCBMSG_H

#include "abstractdevicenodepcbmsg.h"

class DeviceNodePcbMsg : public AbstractDeviceNodePcbMsg
{
public:
    int open(const QString &devNodeFileName) override;
    void close() override;
    void enableFasync() override;

private:
    int m_devFileDescriptor = -1;
    QString m_devNodeFileName;
};

#endif // DEVICENODEPCBMSG_H
