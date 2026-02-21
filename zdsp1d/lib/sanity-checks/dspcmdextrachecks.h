#ifndef DSPCMDEXTRACHECKS_H
#define DSPCMDEXTRACHECKS_H

#include "dspvarresolver.h"

class DspCmdExtraChecks
{
public:
    static bool AVERAGE1(const QStringList &paramNames, const short i16Params[],
                         DspVarResolver* varResolver);
    static bool CLEARN(const QStringList &paramNames, const short i16Params[],
                       DspVarResolver* varResolver);
    static bool COPYMEM(const QStringList &paramNames, const short i16Params[],
                        DspVarResolver* varResolver);
};

#endif // DSPCMDEXTRACHECKS_H
