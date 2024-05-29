#ifndef ADJFLAGS_H
#define ADJFLAGS_H

static constexpr int GainCorrOrder = 3; // ax^3 + bx^2 + cx + d
static constexpr int PhaseCorrOrder  = 3;
static constexpr int OffsetCorrOrder = 3;

namespace Adjustment
{
    enum jDataStatus
    {
        adjusted,
        notAdjusted,
        wrongVERS = 2,
        wrongSNR = 4
    };
    enum StatusMasks
    {
        AdjustedMask = 128
    };
}

#endif // ADJFLAGS_H
