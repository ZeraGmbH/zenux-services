#include "dspvaroffsetcalc.h"

// TODO:
// * Should negative offsets be rejected?
// * Should hex values without 0x be rejected?

bool DspVarOffsetCalc::calcVarOffset(const QString &varName, const QString &varWithOffset, int &calcedOffset)
{
    QString varWithOffsetWork = varWithOffset;
    varWithOffsetWork.remove(" ");
    QString varNameWork = varName;
    varNameWork.remove(" ");

    if (!varWithOffsetWork.startsWith(varNameWork))
        return false;

    const QString offsetStr = varWithOffsetWork.mid(varNameWork.size());
    if (offsetStr.size() == 0) {
        calcedOffset = 0;
        return true;
    }

    return calcOffset(offsetStr, calcedOffset);
}

bool DspVarOffsetCalc::calcOffset(const QString &offsetStr, int &calcedOffset)
{
    bool ok = false;
    int offset = offsetStr.toInt(&ok, 10); // dec
    if (!ok)
        offset = offsetStr.toInt(&ok, 16); // hex

    if (ok)
        calcedOffset = offset;
    return ok;
}
