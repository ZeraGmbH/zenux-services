#ifndef MOCKATMELRANGES_H
#define MOCKATMELRANGES_H

#include "atmelcommon.h"

class MockAtmelRanges : public AtmelRanges
{
public:
    MockAtmelRanges();
    AtmelCommon::atmelRM readRange(quint8 channel, quint8& range) override;
    AtmelCommon::atmelRM setRange(quint8 channel, quint8 range) override;
};

#endif // MOCKATMELRANGES_H
