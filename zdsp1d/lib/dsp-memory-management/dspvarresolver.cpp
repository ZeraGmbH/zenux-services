#include "dspvarresolver.h"

DspVarResolver::DspVarResolver()
{
    m_varHash.clear();
    m_varParser.SetDelimiter("(,+,-,)"); // setze die trennzeichen für den parser
    m_varParser.SetWhiteSpace(" (,)");
}

void DspVarResolver::setVarHash()
{
    m_varHash.clear();
    for (int i = 0; i < MemSectionList.count(); i++) {
        initMemsection(MemSectionList.at(i));
        setQHash(MemSectionList.at(i));
    }
}

void DspVarResolver::addSection(TMemSection* section)
{
    MemSectionList.append(section);
}

void DspVarResolver::initMemsection(TMemSection *psec)
{
    if (psec->Section == systemSection) { // wir initialisieren nur system sections
        long offs = 0;
        for (int i = 0; i< (psec->n); i++) {
            psec->DspVar[i].offs = offs;
            psec->DspVar[i].adr = psec->StartAdr + offs;
            offs += psec->DspVar[i].size;
        }
    }
}

void DspVarResolver::setQHash(TMemSection* psec) // zum setzen der qhash
{
    for (int i = 0; i< (psec->n); i++)
        m_varHash[psec->DspVar[i].Name] = &(psec->DspVar[i]);
}

long DspVarResolver::calcOffsetFromStr(const QString &str)
{
    bool ok;
    long offset = str.toLong(&ok, 10); // prüfen auf dez. konstante
    if(ok)
        return offset;
    offset = str.toLong(&ok, 16); // mal hex versuchen
    if(ok)
        return offset;
    return -1; // sonst ist das ein fehler
}

long DspVarResolver::varOffset(const QString& varNameWithOffset, ulong userMemOffset, ulong globalstartadr)
{
    TDspVar* dspVar = getDspVar(varNameWithOffset);
    if(dspVar) {
        QString varUpper = varNameWithOffset.toUpper();
        varUpper = varUpper.remove(' ');
        varUpper = varUpper.remove(dspVar->Name);
        ulong retoffs = dspVar->offs;
        if (varUpper.length() > 0) { // wenn noch was da, dann muss das ein +/- offset sein
            long offset = calcOffsetFromStr(varUpper);
            if(offset < 0)
                return -1;
            retoffs += offset;
        }
        if (dspVar->segment == globalSegment) // wenn daten im globalen segment liegen
            retoffs += (globalstartadr - userMemOffset);
        return retoffs;
    }
    // offset only e.g DSPMEMOFFSET
    return calcOffsetFromStr(varNameWithOffset);
}

long DspVarResolver::varAddress(const QString &varNameWithOffset)
{
    TDspVar* dspVar = getDspVar(varNameWithOffset);
    if(dspVar) {
        QString varUpper = varNameWithOffset.toUpper();
        varUpper = varUpper.remove(' ');
        varUpper = varUpper.remove(dspVar->Name);
        if (varUpper.length() > 0) { // wenn noch was da, dann muss das ein +/- offset sein
            long offset = calcOffsetFromStr(varUpper);
            if(offset >= 0)
                return dspVar->adr + offset;
            return -1;
        }
        return dspVar->adr;
    }
    // offset only e.g DSPMEMOFFSET
    return calcOffsetFromStr(varNameWithOffset);
}

TDspVar* DspVarResolver::getDspVar(const QString &varNameWithOffset)
{
    QString upperName = varNameWithOffset.toUpper();
    const QChar* cts = upperName.data();
    QString baseVarName = m_varParser.GetKeyword(&cts);
    auto iter = m_varHash.constFind(baseVarName);
    if(iter != m_varHash.constEnd())
        return iter.value();
    return nullptr;
}

int DspVarResolver::type(const QString &varNameWithOffset)
{
    TDspVar* var = getDspVar(varNameWithOffset);
    if(var)
        return var->type;
    return eUnknown;
}
