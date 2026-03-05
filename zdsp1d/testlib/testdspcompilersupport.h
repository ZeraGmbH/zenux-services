#ifndef TESTDSPCOMPILERSUPPORT_H
#define TESTDSPCOMPILERSUPPORT_H

#include <abstractdspcompilersupport.h>

class TestDspCompilerSupport : public AbstractDspCompilerSupport
{
public:
    void startClientArea(int entityId, QString additionalInfo, MemType memType) override;
    void clearGlobalForAllCmds() override;
    static const QStringList &getRawDspCommandsAllVerbose(MemType memType);
    static int getRawDspCommandsCount(MemType memType);

    bool addCmdToRaw(const QString &dspCmdLine,
                     const QStringList &paramNames, const short i16Params[],
                     const DspCmdDecodingDetails *dspcmd,
                     DspVarResolver* varResolver) override;
    bool addCmdToRaw1Param(const QString &dspCmdLine,
                           ulong par,
                           const DspCmdDecodingDetails *dspcmd) override;
    bool addCmdToRaw2Params(const QString &dspCmdLine, const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2) override;
    const QStringList &getRawDspCommands(MemType memType) const override;

private:
    void appendCmdRaw(const QString &cmdRaw);
    static bool allChecks(const QString &dspCmdLine, const QStringList &paramNames,
                          const short i16Params[],
                          const DspCmdDecodingDetails *dspcmd,
                          DspVarResolver* varResolver);
    static bool syntaxCheck(const QString& dspCmdLine);

    MemType m_currMemType = CYCLIC;
    QStringList m_rawDspCommandsClientCyclic;
    QStringList m_rawDspCommandsClientInterrupt;
    static QStringList m_rawDspCommandsAllCyclic;
    static QStringList m_rawDspCommandsAllInterrupt;
};

#endif // TESTDSPCOMPILERSUPPORT_H
