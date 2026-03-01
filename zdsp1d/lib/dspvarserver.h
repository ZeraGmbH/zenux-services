#ifndef DSPVARSERVER_H
#define DSPVARSERVER_H

#include "dspvarclientinterface.h"
#include <QString>

struct DspVarServer {
    bool setupFromCommaSeparatedString(const QString& varDefinition);

    QString m_clientHandleName;
    QString Name;                           // name der variablen
    ushort size = 0;                        // anzahl worte
    DspDataType type = dspDataTypeUnknown;
    ulong adr = 0;                          // die abs. adresse auf welcher sich die variable befindet
    ulong offs = 0;                         // der offset innerhalb der memory section
    DspSegmentType segment = moduleLocalSegment;
    static QString toHex(int val);
};

#endif // DSPVARSERVER_H
