#ifndef ABSTRACTDSPCOMPILERSUPPORT_H
#define ABSTRACTDSPCOMPILERSUPPORT_H

#include "dspapi.h"
#include <QStringList>
#include <memory>

class AbstractDspCompilerSupport
{
public:
    virtual ~AbstractDspCompilerSupport() = default;

    enum MemType {
        CYCLIC,
        INTERRUPT
    };
    virtual void startClientArea(int entityId, QString additionalInfo, MemType memType) = 0;
    virtual void clearTotalCmdLists() = 0;

    virtual bool addCmdToRaw(const QString &dspCmdLine,
                             const QStringList &paramNames, const short i16Params[],
                             const DspCmdDecodingDetails *dspcmd,
                             DspVarResolver* varResolver) = 0;
    virtual bool addCmdToRaw1Param(const QString &dspCmdLine,
                                   ulong par,
                                   const DspCmdDecodingDetails *dspcmd) = 0;
    virtual bool addCmdToRaw2Params(const QString &dspCmdLine,
                                    const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2) = 0;
    virtual const QStringList &getRawDspCommands(MemType memType) const = 0;
};

typedef std::shared_ptr<AbstractDspCompilerSupport> AbstractDspCompilerSupportPtr;

#endif // ABSTRACTDSPCOMPILERSUPPORT_H
