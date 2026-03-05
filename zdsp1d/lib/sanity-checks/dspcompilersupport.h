#ifndef DSPCOMPILERSUPPORT_H
#define DSPCOMPILERSUPPORT_H

#include "abstractdspcompilersupport.h"

class DspCompilerSupport : public AbstractDspCompilerSupport
{
public:
    void startClientArea(int entityId, QString additionalInfo, MemType memType) override;
    void clearGlobalForAllCmds() override {}

    bool addCmdToRaw(const QString &dspCmdLine,
                     const QStringList &paramNames, const short i16Params[],
                     const DspCmdDecodingDetails *dspcmd,
                     DspVarResolver* m_varResolver) override;
    bool addCmdToRaw1Param(const QString &dspCmdLine,
                           ulong par,
                           const DspCmdDecodingDetails *dspcmd) override;
    bool addCmdToRaw2Params(const QString &dspCmdLine,
                            const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2) override;
    const QStringList &getRawDspCommands(MemType memType) const override;
private:
    const QStringList m_dummyEmptyCmdList;
};

#endif // DSPCOMPILERSUPPORT_H
