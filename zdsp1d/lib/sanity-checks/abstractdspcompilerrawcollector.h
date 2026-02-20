#ifndef ABSTRACTDSPCOMPILERRAWCOLLECTOR_H
#define ABSTRACTDSPCOMPILERRAWCOLLECTOR_H

#include "dspapi.h"
#include <QStringList>
#include <memory>

class AbstractDspCompilerRawCollector
{
public:
    virtual ~AbstractDspCompilerRawCollector() = default;
    virtual void addCmd(const DspCmdDecodingDetails *dspcmd) = 0;
    virtual void addCmd1Param(const DspCmdDecodingDetails *dspcmd, ulong par) = 0;
    virtual void addCmd2Params(const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2) = 0;
    virtual void addCmd3Params(const DspCmdDecodingDetails *dspcmd, ulong par1, ulong par2, ulong par3) = 0;
    virtual const QStringList &getRawDspCommands() const = 0;
};

typedef std::shared_ptr<AbstractDspCompilerRawCollector> AbstractDspCompilerRawCollectorPtr;

#endif // ABSTRACTDSPCOMPILERRAWCOLLECTOR_H
