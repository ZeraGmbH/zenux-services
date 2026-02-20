#include "dspcmdextrachecks.h"

bool DspCmdExtraChecks::CLEARN(const QStringList &paramNames, const short i16Params[],
                                    DspVarResolver *varResolver)
{
    int len = i16Params[0];
    TDspVar* dspVar = varResolver->getDspVar(paramNames[1]);
    if (dspVar) {
        int size = dspVar->size;
        if (len > size) {
            qWarning("Overflow on %s buffer size %i wanted %i",
                     qPrintable(paramNames[1]), size, len);
            return false;
        }
    }
    return true;
}
