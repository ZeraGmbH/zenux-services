#include "dspcompilerrawcollector.h"

void DspCompilerRawCollector::addCmd(const DspCmdDecodingDetails *dspcmd)
{
    m_rawDspCommands.append(QString(dspcmd->Name) + "()");
}

void DspCompilerRawCollector::addCmd1Param(const DspCmdDecodingDetails *dspcmd, ulong par)
{
    QString hexPar = TDspVar::toHex(par);
    m_rawDspCommands.append(QString("%1(%2)").arg(QString(dspcmd->Name), hexPar));
}

void DspCompilerRawCollector::addCmd2Params(const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2)
{
    QString hexPar1 = TDspVar::toHex(par1);
    QString hexPar2 = TDspVar::toHex(par2);
    m_rawDspCommands.append(QString("%1(%2,%3)").arg(QString(dspcmd->Name), hexPar1, hexPar2));
}

void DspCompilerRawCollector::addCmd3Params(const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2, ulong par3)
{
    QString hexPar1 = TDspVar::toHex(par1);
    QString hexPar2 = TDspVar::toHex(par2);
    QString hexPar3 = TDspVar::toHex(par3);
    m_rawDspCommands.append(QString("%1(%2,%3,%4)").arg(QString(dspcmd->Name), hexPar1, hexPar2, hexPar3));
}

const QStringList &DspCompilerRawCollector::getRawDspCommands() const
{
    return m_rawDspCommands;
}
