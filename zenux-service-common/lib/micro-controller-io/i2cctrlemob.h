#ifndef I2CCTRLEMOB_H
#define I2CCTRLEMOB_H

#include "abstractalli2ccontrollers.h"
#include <i2cmultiplexerfactory.h>

class I2cCtrlEMOB : public AbstractI2cCtrlEMOB
{
public:
    I2cCtrlEMOB(QString deviceNodeName, quint8 i2cAddress,
                quint8 i2cAddressMux, qint8 muxChannel,
                quint8 debugLevel);
    ZeraMControllerIo::atmelRM sendPushbuttonPress() override;
    ZeraMControllerIo::atmelRM readEmobLockState(quint8 &status) override;
    ZeraMControllerIo::atmelRM readEmobInstrumentSubType(QString& answer) override;
    ZeraMControllerIo::atmelRM readEmobErrorStatus(quint8& err) override;
    ZeraMControllerIo::atmelRM clearErrorStatus() override;
    ZeraMControllerIo::atmelRM readExchangeData(QByteArray &answer) override;
    ZeraMControllerIo::atmelRM writeExchangeData(QByteArray &data, int senderId) override;
private:
    ZeraMControllerIo m_ctrlIo;
    I2cMuxerInterface::Ptr m_i2cMuxer;
};

#endif // I2CCTRLEMOB_H
