#ifndef ADJEEPROMTOOLS_H
#define ADJEEPROMTOOLS_H

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

#endif // ADJEEPROMTOOLS_H
