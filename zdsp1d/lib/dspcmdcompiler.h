#ifndef DSPCMDCOMPILER_H
#define DSPCMDCOMPILER_H

#include "dspapi.h"
#include "dspvarresolver.h"

class DspCmdCompiler
{
public:
    DspCmdCompiler(DspVarResolver* varResolver, int dspInterruptId);
    DspCmdWithParamsRaw compileOneCmdLineZeroAligned(const QString &cmdLine,
                                                     bool* ok);
    DspCmdWithParamsRaw compileOneCmdLineAligned(const QString &cmdLine,
                                                 bool* ok,
                                                 ulong userMemOffset,
                                                 ulong globalstartadr);
    bool compileCmds(const QString& cmdsSemicolonSeparated,
                     QList<DspCmdWithParamsRaw> &genCmdList,
                     QString& err,
                     ulong userMemOffset,
                     ulong globalstartadr);

private:
    static bool syntaxCheck(const QString& dspCmdLine);
    bool overflowCheckCMD2i16(const DspCmdDecodingDetails *decodedCmd,
                              const QStringList &twoParamNames, const short twoI16Params[2]);
    DspVarResolver* m_varResolver;
    const int m_dspInterruptId;
};

#endif // DSPCMDCOMPILER_H
