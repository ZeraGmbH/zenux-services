#ifndef DSPVAROFFSETCALC_H
#define DSPVAROFFSETCALC_H

#include <QString>

class DspVarOffsetCalc
{
public:
    static bool calcVarOffset(const QString &varName, const QString varWithOffset, int &calcedOffset);
    static bool calcOffset(const QString offsetStr, int &calcedOffset);
};

#endif // DSPVAROFFSETCALC_H
