#ifndef DSPCMDCOMPILER_H
#define DSPCMDCOMPILER_H

#include "dsp.h"
#include "dspvarresolver.h"

class DspCmdCompiler
{
public:
    static DspCmdWithParamsRaw compileOneCmdLine(QString cmdLine,
                                                 bool* ok,
                                                 DspVarResolver* varResolver,
                                                 int socket,
                                                 ulong userMemOffset,
                                                 ulong globalstartadr);
private:
    static bool syntaxCheck(const QString& dspCmdLine);
};

#endif // DSPCMDCOMPILER_H
