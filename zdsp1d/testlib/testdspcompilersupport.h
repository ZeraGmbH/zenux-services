#ifndef TESTDSPCOMPILERSUPPORT_H
#define TESTDSPCOMPILERSUPPORT_H

#include <abstractdspcompilersupport.h>

class TestDspCompilerSupport : public AbstractDspCompilerSupport
{
public:
    bool addCmdToRaw(const QString &dspCmdLine,
                     const QStringList &paramNames, const short i16Params[],
                     const DspCmdDecodingDetails *dspcmd,
                     DspVarResolver* varResolver) override;
    bool addCmdToRaw1Param(const QString &dspCmdLine,
                           ulong par,
                           const DspCmdDecodingDetails *dspcmd) override;
    bool addCmdToRaw2Params(const QString &dspCmdLine, const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2) override;
    const QStringList &getRawDspCommands() const override;

private:
    static bool allChecks(const QString &dspCmdLine, const QStringList &paramNames,
                          const short i16Params[],
                          const DspCmdDecodingDetails *dspcmd,
                          DspVarResolver* varResolver);
    static bool syntaxCheck(const QString& dspCmdLine);


    QStringList m_rawDspCommands;
};

#endif // TESTDSPCOMPILERSUPPORT_H
