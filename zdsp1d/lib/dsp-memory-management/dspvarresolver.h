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
    void setVarHash();
    void addSection(TMemSection* section);
    long varOffset(const QString &varNameWithOffset, ulong userMemOffset, ulong globalstartadr);
    long varAddress(const QString& varNameWithOffset);
    TDspVar* getDspVar(const QString& varNameWithOffset);
    int type(const QString &varNameWithOffset);

private:
    void initMemsection(TMemSection* psec);
    void setQHash(TMemSection* psec);
    static long calcOffsetFromStr(const QString &str);
    QHash<QString, TDspVar*> m_varHash;
    cParse m_varParser;
    QList<TMemSection*> MemSectionList;
};

#endif // DSPVARRESOLVER_H
