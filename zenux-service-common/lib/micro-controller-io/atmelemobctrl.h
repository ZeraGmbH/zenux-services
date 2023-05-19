#ifndef ATMELEMOBCTRL_H
#define ATMELEMOBCTRL_H

#include <QString>
#include <memory>
#include <atmelcommon.h>
#include <atmelcommonversions.h>
#include <i2cmultiplexerfactory.h>

class AtmelEmobCtrl : public AtmelCommonVersions
{
public:
    AtmelEmobCtrl(ZeraMcontrollerBasePtr i2cCtrl, QString devnode, quint8 adrMux, quint8 muxChannel);
    ZeraMcontrollerBase::atmelRM readCTRLVersion(QString& answer) override;
    ZeraMcontrollerBase::atmelRM readPCBVersion(QString& answer) override;
private:
    ZeraMcontrollerBasePtr m_i2cCtrl;
    quint8 m_ctrlChannel;
    I2cMuxerInterface::Ptr m_i2cMuxer;
};

#endif // ATMELEMOBCTRL_H
