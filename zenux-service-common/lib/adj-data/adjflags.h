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
}

#endif // ADJFLAGS_H
