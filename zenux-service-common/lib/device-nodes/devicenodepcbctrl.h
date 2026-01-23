#ifndef DEVICENODEPCBCTRL_H
#define DEVICENODEPCBCTRL_H

#include "abstractdevicenodepcbctrl.h"
#include <fcntl.h>
#include <unistd.h>

class DeviceNodePcbCtrl : public AbstractDeviceNodePcbCtrl
{
public:
    int open(QString devNodeFileName) override;
    void close() override;
    void enableFasync() override;

private:
    int m_devFileDescriptor = -1;
    QString m_devNodeFileName;
};

#endif // DEVICENODEPCBCTRL_H
