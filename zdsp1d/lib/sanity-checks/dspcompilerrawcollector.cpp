#include "dspcompilerrawcollector.h"

void DspCompilerRawCollector::addCmd(const DspCmdDecodingDetails *dspcmd)
{
    Q_UNUSED(dspcmd)
}

void DspCompilerRawCollector::addCmd1Param(const DspCmdDecodingDetails *dspcmd, ulong par)
{
    Q_UNUSED(dspcmd)
    Q_UNUSED(par)
}

void DspCompilerRawCollector::addCmd2Params(const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2)
{
    Q_UNUSED(dspcmd)
    Q_UNUSED(par1)
    Q_UNUSED(par2)
}

void DspCompilerRawCollector::addCmd3Params(const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2, ulong par3)
{
    Q_UNUSED(dspcmd)
    Q_UNUSED(par1)
    Q_UNUSED(par2)
    Q_UNUSED(par3)
}

const QStringList &DspCompilerRawCollector::getRawDspCommands() const
{
    return m_rawDspCommands;
}
