#ifndef PCBDEVICENODEMESSAGE_H
#define PCBDEVICENODEMESSAGE_H

#include "pcbdevicenodemessageabstract.h"

class PcbDeviceNodeMessage : public PcbDeviceNodeMessageAbstract
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
