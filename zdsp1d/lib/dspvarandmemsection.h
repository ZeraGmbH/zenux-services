#ifndef DSPVARANDMEMSECTION_H
#define DSPVARANDMEMSECTION_H

#include "dspvar.h"
#include <QString>

struct TDspVar {
    bool Init(const QString& varDefinition);
    QString m_clientHandleName;
    QString Name;                           // name der variablen
    ushort size = 0;                        // anzahl worte
    DspDataType type = dspDataTypeUnknown;
    ulong adr = 0;                          // die abs. adresse auf welcher sich die variable befindet
    ulong offs = 0;                         // der offset innerhalb der memory section
    DspSegmentType segment = moduleLocalSegment;
    static QString toHex(int val);
};

struct TMemSection { // beschreibt eine dsp memory section
    TMemSection() = default;
    TMemSection(long startAddress,
                int varCount,
                TDspVar *dspVars);
    long m_startAddress = 0;
    int m_varCount = 0;
    TDspVar *m_dspVars = nullptr;
};

#endif // DSPVARANDMEMSECTION_H
