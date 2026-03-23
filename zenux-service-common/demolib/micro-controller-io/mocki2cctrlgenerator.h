#ifndef MOCKI2CCTRLGENERATOR_H
#define MOCKI2CCTRLGENERATOR_H

#include "abstractalli2ccontrollers.h"

class MockI2cCtrlGenerator : public AbstractI2cCtrlGenerator
{
public:
    MockI2cCtrlGenerator();
    ZeraMControllerIo::atmelRM sendSourceModeOn(QList<ChannelDefinitions> channelsOn) override;
    ZeraMControllerIo::atmelRM sendSourceOn(QList<ChannelDefinitions> channelsOn) override;
    ZeraMControllerIo::atmelRM sendSourceAmplitudeChangeRange(float amplitude, ChannelDefinitions channel) override;
};

#endif // MOCKI2CCTRLGENERATOR_H
