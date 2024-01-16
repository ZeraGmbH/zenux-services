#ifndef MOCKATMELMMODES_H
#define MOCKATMELMMODES_H

#include "abstracti2ccontrollers.h"

class MockAtmelMModes : public AtmelMModes
{
public:
    MockAtmelMModes();
    ZeraMControllerIo::atmelRM setMeasMode(quint8 mmode) override;
    ZeraMControllerIo::atmelRM readMeasMode(quint8& mmode) override;
};

#endif // MOCKATMELMMODES_H
