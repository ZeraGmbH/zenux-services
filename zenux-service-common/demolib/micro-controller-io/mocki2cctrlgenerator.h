#ifndef MOCKI2CCTRLGENERATOR_H
#define MOCKI2CCTRLGENERATOR_H

#include "abstractalli2ccontrollers.h"

class MockI2cCtrlGenerator : public AbstractI2cCtrlGenerator
{
public:
    MockI2cCtrlGenerator(QStringList &channelMNamesModeOn,
                         QStringList &channelMNamesOn);
    ZeraMControllerIo::atmelRM readSourceModeOn(QStringList &channelMNamesModeOnRead) override;
    ZeraMControllerIo::atmelRM sendSourceModeOn(const QStringList &channelMNamesModeOn) override;
    ZeraMControllerIo::atmelRM readSourceOn(QStringList &channelMNamesOnRead) override;
    ZeraMControllerIo::atmelRM sendSourceOn(const QStringList &channelMNamesOn) override;
    ZeraMControllerIo::atmelRM sendSourceAmplitudeChangeRange(float amplitude, const QString &channelMName) override;

private:
    QStringList &m_channelMNamesModeOn;
    QStringList &m_channelMNamesOn;
};

#endif // MOCKI2CCTRLGENERATOR_H
