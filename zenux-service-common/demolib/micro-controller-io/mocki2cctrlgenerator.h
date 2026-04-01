#ifndef MOCKI2CCTRLGENERATOR_H
#define MOCKI2CCTRLGENERATOR_H

#include "abstractalli2ccontrollers.h"
#include "controllerpersitentdata.h"

class MockI2cCtrlGenerator : public AbstractI2cCtrlGenerator
{
public:
    MockI2cCtrlGenerator(ControllerPersitentData::TPersitentControllerData &persistentData);

    ZeraMControllerIo::atmelRM readSourceModeOn(QStringList &channelMNamesModeOnRead) override;
    ZeraMControllerIo::atmelRM sendSourceModeOn(const QStringList &channelMNamesModeOn) override;

    ZeraMControllerIo::atmelRM readSourceOn(QStringList &channelMNamesOnRead) override;
    ZeraMControllerIo::atmelRM sendSourceOn(const QStringList &channelMNamesOn) override;

    ZeraMControllerIo::atmelRM setRangeByAmplitude(const QString &channelMName, float amplitude) override;
    ZeraMControllerIo::atmelRM readRange(const QString& channelMName, quint8& range) override;
    ZeraMControllerIo::atmelRM setRange(const QString& channelMName, quint8 range) override;

    ZeraMControllerIo::atmelRM setDspAmplitude(const QString& channelMName, float amplitude) override;
    ZeraMControllerIo::atmelRM getDspAmplitude(const QString& channelMName, float &amplitude) override;

    ZeraMControllerIo::atmelRM tunnelToDsp(const QString& channelMName, const QByteArray &cmd, QByteArray &response) override;

private:
    QStringList &m_channelMNamesModeOn;
    QStringList &m_channelMNamesOn;
    ControllerPersitentData::RangeMap &m_generatorRangeMap;
    ControllerPersitentData::DspAmplitudeMap &m_dspAmplitudeMap;
};

#endif // MOCKI2CCTRLGENERATOR_H
