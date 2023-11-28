#ifndef DSPVARRESOLVER_H
#define DSPVARRESOLVER_H

#include "dspvardefinitions.h"
#include "dspvarparser.h"
#include <QString>
#include <QHash>

class DspVarResolver
{
public:
    DspVarResolver();
    void setVarHash();
    void addSection(TMemSection* section); // sections werden im konstruktor erstmal fest eingebunden
    long offs(const QString &varNameOrValue, ulong umo, ulong globalstartadr); // gibt die offs. adresse einer variablen zurück, -1 wenn es die variable nicht gibt, zum generiren der dsp kommandos
    long adr(QString& varNameOrValue); // gibt die abs. adresse einer variablen im dsp zurück, -1 wenn es die variable nicht gibt, zum schreiben in den dsp
    TDspVar* getDspVar(const QString& varName); //  gibt einen zeiger auf TDspVar zurück, zum lesen von daten aus dem dsp
    int type(const QString &varName);

private:
    QHash<QString, TDspVar*> m_varHash;
    DspVarParser VarParser;
    void initMemsection(TMemSection* psec); // zum initialisieren einer memory section
    void setQHash(TMemSection* psec); // zum setzen der qhash
    QList<TMemSection*> MemSectionList;
};

#endif // DSPVARRESOLVER_H
