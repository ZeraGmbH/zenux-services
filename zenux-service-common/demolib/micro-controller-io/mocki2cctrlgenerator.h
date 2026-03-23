#ifndef MOCKI2CCTRLGENERATOR_H
#define MOCKI2CCTRLGENERATOR_H

#include "abstractalli2ccontrollers.h"

class MockI2cCtrlGenerator : public AbstractI2cCtrlGenerator
{
public:
    MockI2cCtrlGenerator();
    ZeraMControllerIo::atmelRM sendSourceModeOn(const QStringList &channelMNamesOn) override;
    ZeraMControllerIo::atmelRM sendSourceOn(const QStringList &channelMNamesOn) override;
    ZeraMControllerIo::atmelRM sendSourceAmplitudeChangeRange(float amplitude, const QString &channelMName) override;
};

#endif // MOCKI2CCTRLGENERATOR_H
