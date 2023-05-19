#ifndef ATMELEMOBCTRL_H
#define ATMELEMOBCTRL_H

#include <QString>
#include <memory>
#include <atmelcommon.h>
#include <atmelcommonversions.h>
#include <zeramcontrollerbootloaderstopper.h>
#include <i2cmultiplexerfactory.h>

class AtmelEmobCtrl : public AtmelCommonVersions
{
public:
    AtmelEmobCtrl(ZeraMcontrollerIoPtr i2cCtrl, QString devnode, quint8 adrMux, quint8 muxChannel);
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM readPCBVersion(QString& answer) override;
private:
    ZeraMcontrollerIoPtr m_i2cCtrl;
    quint8 m_ctrlChannel;
    I2cMuxerInterface::Ptr m_i2cMuxer;
    ZeraMControllerBootloaderStopper m_bootloaderStopper;
};

#endif // ATMELEMOBCTRL_H
