#ifndef ZDSPDUMPFUNCTIONS_H
#define ZDSPDUMPFUNCTIONS_H

#include <zdspserver.h>
#include <QJsonObject>

class ZDspDumpFunctions
{
public:
    static QJsonObject getStaticMemAllocation();
    static QJsonObject getMemoryDump(const ZDspServer *server);
    static quint32 getDspCmdListCompiledCrc(const QList<DspCmdWithParamsCompiled> &cmdList);
    struct VarLocation
    {
        QString m_variableName;
        ulong m_localVariableAddress;
        ulong m_absoluteVariableAddress;
    };
    static QList<VarLocation> getLocalVariableDump(const DspMemorySectionInternal &clientUserMemSection);
    static QList<VarLocation> getGlobalVariableDump(const DspMemorySectionInternal &clientUserMemSection);
private:
    static QString crcToHex(quint32 val);
};

#endif // ZDSPDUMPFUNCTIONS_H
