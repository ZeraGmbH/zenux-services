#ifndef DSPDSPCOMPILERSUPPORT_H
#define DSPDSPCOMPILERSUPPORT_H

#include "abstractdspcompilersupport.h"

class DspDspCompilerSupport : public AbstractDspCompilerSupport
{
public:
    bool addCmdToRaw(const QString &dspCmdLine,
                     const QStringList &paramNames, const short i16Params[],
                     const DspCmdDecodingDetails *dspcmd,
                     DspVarResolver* m_varResolver) override;
    bool addCmdToRaw1Param(const QString &dspCmdLine,
                           ulong par,
                           const DspCmdDecodingDetails *dspcmd) override;
    bool addCmdToRaw2Params(const QString &dspCmdLine,
                            const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2) override;
    const QStringList &getRawDspCommands() const override;
private:
    QStringList m_rawDspCommands;
};

#endif // DSPDSPCOMPILERSUPPORT_H
