#ifndef ATMELEMOBCTRL_H
#define ATMELEMOBCTRL_H

#include "abstracti2ccontrollers.h"
#include <QString>
#include <memory>
#include <i2cmultiplexerfactory.h>

class AtmelCtrlEmob : public AbstractI2cCtrlCommonVersions
{
public:
    AtmelCtrlEmob(ZeraMcontrollerIoPtr i2cCtrl, QString devnode, quint8 adrMux, quint8 muxChannel);
    ZeraMControllerIo::atmelRM readCTRLVersion(QString& answer) override;
    ZeraMControllerIo::atmelRM readPCBVersion(QString& answer) override;
private:
    ZeraMcontrollerIoPtr m_i2cCtrl;
    quint8 m_ctrlChannel;
    I2cMuxerInterface::Ptr m_i2cMuxer;

    QString m_ctrlVersion;
    QString m_pcbVersion;
};

#endif // ATMELEMOBCTRL_H
