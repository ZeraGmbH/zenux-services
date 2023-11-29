#ifndef DSPVARDEFINITIONS_H
#define DSPVARDEFINITIONS_H

#include <QString>

enum dType { eInt, eFloat, eUnknown};

enum sectionType { systemSection, userSection};
enum segmentType { localSegment, globalSegment};

struct TDspVar { // dient ebenfalls der dekodierung
    QString Name; // name der variablen
    ushort size;  // anzahl worte
    dType type; //
    ulong adr; // die abs. adresse auf welcher sich die variable befindet
    ulong offs; // der offset innerhalb der memory section
    segmentType segment; // segment info, nur relevant für client sections
};

struct TMemSection { // beschreibt eine dsp memory section
    sectionType Section;
    long StartAdr;
    int n; // anzahl der sdspvar elemente
    TDspVar *DspVar;
};

#endif // DSPVARDEFINITIONS_H
