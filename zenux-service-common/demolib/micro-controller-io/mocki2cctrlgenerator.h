#ifndef MOCKI2CCTRLGENERATOR_H
#define MOCKI2CCTRLGENERATOR_H

#include "abstractalli2ccontrollers.h"

class MockI2cCtrlGenerator : public AbstractI2cCtrlGenerator
{
public:
    MockI2cCtrlGenerator();
    ZeraMControllerIo::atmelRM sendSourceModeOn(QList<ChannelDefinitions> channelsOn) override;
};

#endif // MOCKI2CCTRLGENERATOR_H
