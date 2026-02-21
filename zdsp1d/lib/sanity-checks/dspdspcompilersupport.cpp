#include "dspdspcompilersupport.h"

bool DspDspCompilerSupport::addCmdToRaw(const QString &dspCmdLine,
                                        const QStringList &paramNames,
                                        const short i16Params[],
                                        const DspCmdDecodingDetails *dspcmd,
                                        DspVarResolver *m_varResolver)
{
    Q_UNUSED(dspCmdLine)
    Q_UNUSED(paramNames)
    Q_UNUSED(i16Params)
    Q_UNUSED(dspcmd)
    Q_UNUSED(m_varResolver)
    return true;
}

bool DspDspCompilerSupport::addCmdToRaw1Param(const QString &dspCmdLine,
                                              ulong par,
                                              const DspCmdDecodingDetails *dspcmd)
{
    Q_UNUSED(dspCmdLine)
    Q_UNUSED(dspcmd)
    Q_UNUSED(par)
    return true;
}

bool DspDspCompilerSupport::addCmdToRaw2Params(const QString &dspCmdLine,
                                               const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2)
{
    Q_UNUSED(dspCmdLine)
    Q_UNUSED(dspcmd)
    Q_UNUSED(par1)
    Q_UNUSED(par2)
    return true;
}

const QStringList &DspDspCompilerSupport::getRawDspCommands() const
{
    return m_rawDspCommands;
}
