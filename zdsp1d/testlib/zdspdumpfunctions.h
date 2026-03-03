#ifndef ZDSPDUMPFUNCTIONS_H
#define ZDSPDUMPFUNCTIONS_H

#include <zdspserver.h>
#include <QJsonObject>

class ZDspDumpFunctions
{
public:
    static QJsonObject getStaticMemAllocation();
    static QJsonObject getMemoryDump(ZDspServer *server);
private:
    static QString crcToHex(quint32 val);
};

#endif // ZDSPDUMPFUNCTIONS_H
