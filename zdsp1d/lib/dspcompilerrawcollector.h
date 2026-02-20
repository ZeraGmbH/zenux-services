#ifndef DSPCOMPILERRAWCOLLECTOR_H
#define DSPCOMPILERRAWCOLLECTOR_H

#include "dspapi.h"
#include <QStringList>

class DspCompilerRawCollector
{
public:
    void addCmd(const DspCmdDecodingDetails *dspcmd);
    void addCmd1Param(const DspCmdDecodingDetails *dspcmd, ulong par);
    void addCmd2Params(const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2);
    void addCmd3Params(const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2, ulong par3);
    const QStringList &getRawDspCommands() const;
private:
    QStringList m_rawDspCommands;
};

#endif // DSPCOMPILERRAWCOLLECTOR_H
