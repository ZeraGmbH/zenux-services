#ifndef DSPCMDCOMPILER_H
#define DSPCMDCOMPILER_H

#include "dsp.h"
#include "dspvarresolver.h"

class DspCmdCompiler
{
public:
    DspCmdCompiler(DspVarResolver* varResolver, int socket);
    DspCmdWithParamsRaw compileOneCmdLine(QString cmdLine,
                                          bool* ok,
                                          ulong userMemOffset,
                                          ulong globalstartadr);
private:
    static bool syntaxCheck(const QString& dspCmdLine);
    DspVarResolver* m_varResolver;
    const int m_socket;
};

#endif // DSPCMDCOMPILER_H
