#ifndef DSPVARINSERVER_H
#define DSPVARINSERVER_H

#include "dspvarclientinterface.h"
#include <QString>
#include <memory>

struct DspVarInServer {
    bool setupFromCommaSeparatedString(const QString& varDefinition);
    static QString toHex(int val);

    QString m_clientHandleName;
    QString Name;                           // name der variablen
    ushort size = 0;                        // anzahl worte
    DspDataType type = dspDataTypeUnknown;
    ulong adr = 0;                          // die abs. adresse auf welcher sich die variable befindet
    ulong offs = 0;                         // der offset innerhalb der memory section
    DspSegmentType segment = moduleLocalSegment;
};

typedef std::shared_ptr<DspVarInServer> DspVarServerPtr;

#endif // DSPVARINSERVER_H
