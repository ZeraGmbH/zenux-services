#ifndef ADJFLAGS_H
#define ADJFLAGS_H

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
