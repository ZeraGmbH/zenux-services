#include "dspcmdextrachecks.h"
#include "dspinterface.h"

bool DspCmdExtraChecks::AVERAGE1(const QStringList &paramNames, const short i16Params[],
                                 DspVarResolver *varResolver)
{
    const QString paramNameTarget = paramNames[2];
    if (paramNameTarget.contains("+") || paramNameTarget.contains("-")) {
        qWarning("Offset used in place you shouldn't?");
        return false;
    }
    const TDspVar* varTransferTarget = varResolver->getDspVar(paramNameTarget);
    if (varTransferTarget) {
        int len = DspBuffLen::avgFilterLen(i16Params[0]);
        int size = varTransferTarget->size;
        if (len > size) {
            qWarning("Overflow on %s buffer size %i+1 wanted %i+1",
                     qPrintable(paramNameTarget), size-1, len-1);
            return false;
        }
    }
    return true;
}

bool DspCmdExtraChecks::CLEARN(const QStringList &paramNames, const short i16Params[],
                               DspVarResolver *varResolver)
{
    const QString paramNameTarget = paramNames[1];
    if (paramNameTarget.contains("+") || paramNameTarget.contains("-")) {
        qWarning("Offset used in place you shouldn't?");
        return false;
    }
    const TDspVar* varTransferTarget = varResolver->getDspVar(paramNameTarget);
    if (varTransferTarget) {
        int len = i16Params[0];
        int size = varTransferTarget->size;
        if (len > size) {
            qWarning("Overflow on %s buffer size %i wanted %i",
                     qPrintable(paramNameTarget), size, len);
            return false;
        }
    }
    return true;
}
