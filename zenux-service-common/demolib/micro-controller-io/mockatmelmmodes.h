#ifndef MOCKATMELMMODES_H
#define MOCKATMELMMODES_H

#include "atmelcommon.h"

class MockAtmelMModes : public AtmelMModes
{
public:
    MockAtmelMModes();
    AtmelCommon::atmelRM setMeasMode(quint8 mmode) override;
    AtmelCommon::atmelRM readMeasMode(quint8& mmode) override;
};

#endif // MOCKATMELMMODES_H
