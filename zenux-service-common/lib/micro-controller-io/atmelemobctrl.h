#ifndef ATMELEMOBCTRL_H
#define ATMELEMOBCTRL_H

#include <QString>
#include <memory>
#include <atmelcommon.h>
#include <atmelcommonversions.h>
#include <i2cmultiplexerfactory.h>

class AtmelEmobCtrl : public ZeraMcontrollerBase, public AtmelCommonVersions
{
public:
    AtmelEmobCtrl(QString devnode, quint8 adr, quint8 ctrlChannelForMux, quint8 debuglevel);
    virtual ~AtmelEmobCtrl() = default;
    atmelRM readCTRLVersion(QString& answer) override;
    atmelRM readPCBVersion(QString& answer) override;
private:
    quint8 m_ctrlChannel;
    I2cMuxerInterface::Ptr m_i2cMuxer;
};

#endif // ATMELEMOBCTRL_H
