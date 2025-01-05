#ifndef DSPVARDEFINITIONS_H
#define DSPVARDEFINITIONS_H

#include <QString>
#include <QVector>

enum dType { eInt, eFloat, eUnknown};

enum sectionType { systemSection, userSection};
enum segmentType { localSegment, globalSegment};

struct TDspVar {
    bool Init(const QString& varDefinition);
    QString m_clientHandleName;
    QString Name;                       // name der variablen
    ushort size = 0;                    // anzahl worte
    dType type = eUnknown;
    ulong adr = 0;                      // die abs. adresse auf welcher sich die variable befindet
    ulong offs = 0;                     // der offset innerhalb der memory section
    segmentType segment = localSegment; // segment info, nur relevant für client sections
};

struct TMemSection { // beschreibt eine dsp memory section
    TMemSection() = default;
    TMemSection(sectionType section, long startAdr, int varCount, TDspVar vars[]);
    sectionType Section = userSection;
    long StartAdr = 0;
    QVector<TDspVar> DspVar;
};

#endif // DSPVARDEFINITIONS_H
