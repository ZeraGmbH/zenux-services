#ifndef MOCKATMELACCUMULATORHANDLER_H
#define MOCKATMELACCUMULATORHANDLER_H

#include "atmelcommon.h"

class MockAtmelAccumulatorHandler : public AtmelAccumulatorHandler
{
public:
    ZeraMControllerIo::atmelRM readAccumulatorStatus(quint8& stat) override;
    ZeraMControllerIo::atmelRM readAccuStateOfCharge(quint8& charge) override;
    ZeraMControllerIo::atmelRM enableTestMode(qint32 testBits) override;
};

#endif // MOCKATMELACCUMULATORHANDLER_H
