#ifndef TESTDSPCOMPILERRAWCOLLECTOR_H
#define TESTDSPCOMPILERRAWCOLLECTOR_H

#include <abstractdspcompilerrawcollector.h>

class TestDspCompilerRawCollector : public AbstractDspCompilerRawCollector
{
public:
    void addCmd(const DspCmdDecodingDetails *dspcmd) override;
    void addCmd1Param(const DspCmdDecodingDetails *dspcmd, ulong par) override;
    void addCmd2Params(const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2) override;
    void addCmd3Params(const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2, ulong par3) override;
    const QStringList &getRawDspCommands() const override;
private:
    QStringList m_rawDspCommands;
};

#endif // TESTDSPCOMPILERRAWCOLLECTOR_H
