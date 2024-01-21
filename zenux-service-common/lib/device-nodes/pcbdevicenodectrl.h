#ifndef PCBDEVICENODECTRL_H
#define PCBDEVICENODECTRL_H

#include "abstractpcbctrldevicenode.h"
#include <fcntl.h>
#include <unistd.h>

class PcbDeviceNodeCtrl : public AbstractPcbCtrlDeviceNode
{
public:
    int open(QString devNodeFileName) override;
    void close() override;
    void enableFasync() override;

private:
    int m_devFileDescriptor = -1;
    QString m_devNodeFileName;
};

#endif // PCBDEVICENODECTRL_H
