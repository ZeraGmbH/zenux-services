#ifndef ZDSPDUMPFUNCTIONS_H
#define ZDSPDUMPFUNCTIONS_H

#include <zdspserver.h>
#include <QJsonObject>

class ZDspDumpFunctions
{
public:
    static QJsonObject getStaticMemAllocation();
    static QJsonObject getFullDump(const ZDspServer *server);
    static QJsonObject getMemoryDump(const ZDspServer *server);
    static quint32 getDspCmdListCompiledCrc(const QList<DspCmdWithParamsCompiled> &cmdList);
    struct VarLocation
    {
        QString m_variableName;
        ulong m_localVariableAddress;
        ulong m_absoluteVariableAddress;
    };
    static QList<VarLocation> getVariableDump(const DspMemorySectionInternal &clientUserMemSection,
                                              DspSegmentType segment);
private:
    static QString crcToHex(quint32 val);
    static QJsonObject dumpVariables(const QList<VarLocation> &vars);
};

#endif // ZDSPDUMPFUNCTIONS_H
