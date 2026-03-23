#ifndef I2CCTRLGENERATOR_H
#define I2CCTRLGENERATOR_H

#include "abstractalli2ccontrollers.h"

class I2cCtrlGenerator : public AbstractI2cCtrlGenerator
{
public:
    I2cCtrlGenerator(const QString &deviceNodeName, quint8 i2cAddress, quint8 debugLevel);
    ZeraMControllerIo::atmelRM sendSourceModeOn(QList<ChannelDefinitions> channelsOn) override;
    ZeraMControllerIo::atmelRM sendSourceOn(QList<ChannelDefinitions> channelsOn) override;
    ZeraMControllerIo::atmelRM sendSourceAmplitudeChangeRange(float amplitude, ChannelDefinitions channel) override;

    static QByteArray convertFloat(float value);
private:
    static quint8 getBitmask(const QList<ChannelDefinitions> &channels);
    static quint8 getControllerInternalChannelNo(ChannelDefinitions channel);

    ZeraMControllerIo m_ctrlIo;
};

#endif // I2CCTRLGENERATOR_H
