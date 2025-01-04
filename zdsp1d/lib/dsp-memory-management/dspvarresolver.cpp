#include "dspvarresolver.h"

DspVarResolver::DspVarResolver()
{
    m_varParser.SetDelimiter("(,+,-,)"); // setze die trennzeichen für den parser
    m_varParser.SetWhiteSpace(" (,)");
}

void DspVarResolver::addSection(TMemSection* section)
{
    MemSectionList.append(section);
}

void DspVarResolver::actualizeVarHash()
{
    m_varHash.clear();
    for(TMemSection* memSection : qAsConst(MemSectionList)) {
        initMemsection(memSection);
        for (int i=0; i<memSection->n; i++)
            m_varHash[memSection->DspVar[i].Name] = &(memSection->DspVar[i]);
    }
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

long DspVarResolver::getVarOffset(const QString& varNameWithOffset, ulong userMemOffset, ulong globalstartadr)
{
    TDspVar* dspVar = getDspVar(varNameWithOffset);
    if(dspVar) {
        ulong retoffs = dspVar->offs;
        QString offsetStr = extractOffset(varNameWithOffset, dspVar->Name);
        if(offsetStr.length() > 0) { // wenn noch was da, dann muss das ein +/- offset sein
            long offset = calcOffsetFromStr(offsetStr);
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

long DspVarResolver::getVarAddress(const QString &varNameWithOffset)
{
    TDspVar* dspVar = getDspVar(varNameWithOffset);
    if(dspVar) {
        QString offsetStr = extractOffset(varNameWithOffset, dspVar->Name);
        if(offsetStr.length() > 0) { // wenn noch was da, dann muss das ein +/- offset sein
            long offset = calcOffsetFromStr(offsetStr);
            if(offset < 0)
                return -1;
            return dspVar->adr + offset;
        }
        return dspVar->adr;
    }
    // offset only e.g DSPMEMOFFSET
    return calcOffsetFromStr(varNameWithOffset);
}

int DspVarResolver::getVarType(const QString &varNameWithOffset)
{
    TDspVar* var = getDspVar(varNameWithOffset);
    if(var)
        return var->type;
    return eUnknown;
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

QString DspVarResolver::extractOffset(const QString &varNameWithOffset, const QString &varName)
{
    QString varUpper = varNameWithOffset.toUpper();
    varUpper.remove(' ');
    return varUpper.remove(varName);
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
