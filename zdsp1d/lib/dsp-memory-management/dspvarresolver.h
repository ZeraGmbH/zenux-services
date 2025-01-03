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
    void addSection(TMemSection* section); // sections werden im konstruktor erstmal fest eingebunden
    long varOffset(const QString &varNameWithOffset, ulong userMemOffset, ulong globalstartadr); // gibt die offs. adresse einer variablen zurück, -1 wenn es die variable nicht gibt, zum generiren der dsp kommandos
    long varAddress(QString& varNameWithOffset); // gibt die abs. adresse einer variablen im dsp zurück, -1 wenn es die variable nicht gibt, zum schreiben in den dsp
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
