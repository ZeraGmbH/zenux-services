#ifndef DSPVARRESOLVER_H
#define DSPVARRESOLVER_H

#include "dspvardefinitions.h"
#include <parse.h>
#include <QString>
#include <QHash>

class DspVarResolver
{
public:
    DspVarResolver();
    void addSection(TMemSection* section);
    void setVarHash();
    TDspVar* getDspVar(const QString& varNameWithOffset);
    long varOffset(const QString &varNameWithOffset, ulong userMemOffset, ulong globalstartadr);
    long varAddress(const QString& varNameWithOffset);
    int type(const QString &varNameWithOffset);

private:
    void initMemsection(TMemSection* psec);
    static long calcOffsetFromStr(const QString &str);
    QHash<QString, TDspVar*> m_varHash;
    cParse m_varParser;
    QList<TMemSection*> MemSectionList;
};

#endif // DSPVARRESOLVER_H
