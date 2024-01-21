#ifndef PCBDEVICENODEMESSAGE_H
#define PCBDEVICENODEMESSAGE_H

#include "abstractpcbmsgdevicenode.h"

class PcbDeviceNodeMessage : public AbstractPcbMsgDeviceNode
{
public:
    int open(QString devNodeFileName) override;
    void close() override;
    void enableFasync() override;

private:
    int m_devFileDescriptor = -1;
    QString m_devNodeFileName;
};

#endif // PCBDEVICENODEMESSAGE_H
