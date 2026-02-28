#ifndef DSPVARANDMEMSECTION_H
#define DSPVARANDMEMSECTION_H

#include "dspvar.h"
#include <QString>

enum sectionType { systemSection, userSection };

struct TDspVar {
    bool Init(const QString& varDefinition);
    QString m_clientHandleName;
    QString Name;                           // name der variablen
    ushort size = 0;                        // anzahl worte
    dspDataType type = dspDataTypeUnknown;
    ulong adr = 0;                          // die abs. adresse auf welcher sich die variable befindet
    ulong offs = 0;                         // der offset innerhalb der memory section
    segmentType segment = localSegment;     // segment info, nur relevant für client sections
    static QString toHex(int val);
};

struct TMemSection { // beschreibt eine dsp memory section
    TMemSection() = default;
    TMemSection(sectionType section,
                long startAddress,
                int varCount,
                TDspVar *dspVars);
    sectionType m_section = userSection;
    long m_startAddress = 0;
    int m_varCount = 0;
    TDspVar *m_dspVars = nullptr;
};

#endif // DSPVARANDMEMSECTION_H
