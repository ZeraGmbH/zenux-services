#ifndef ADJUSTMENTFLAGS_H
#define ADJUSTMENTFLAGS_H

namespace Adjustment
{
    enum jDataStatus
    {
        adjusted,
        notAdjusted,
        wrongVERS = 2,
        wrongSNR = 4
    };
}

#endif // ADJUSTMENTFLAGS_H
