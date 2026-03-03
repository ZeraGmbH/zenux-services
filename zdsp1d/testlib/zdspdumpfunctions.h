#ifndef ZDSPDUMPFUNCTIONS_H
#define ZDSPDUMPFUNCTIONS_H

#include <zdspserver.h>
#include <QJsonObject>

class ZDspDumpFunctions
{
public:
    static QJsonObject getStaticMemAllocation();
};

#endif // ZDSPDUMPFUNCTIONS_H
