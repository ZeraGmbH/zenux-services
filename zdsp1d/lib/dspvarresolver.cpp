#include "dspvarresolver.h"
#include "dspapi.h"

DspVarResolver::DspVarResolver()
{
    m_varParser.SetDelimiter("(,+,-,)"); // setze die trennzeichen für den parser
    m_varParser.SetWhiteSpace(" (,)");
    m_varHash = DspStaticData::getVarHash();
}

void DspVarResolver::addSection(DspMemorySectionInternal* section)
{
    MemSectionList.append(section);
}

void DspVarResolver::actualizeVarHash()
{
    m_varHash = DspStaticData::getVarHash();
    for(DspMemorySectionInternal* memSection : qAsConst(MemSectionList)) {
        DspStaticData::initMemsection(memSection);
        for (int i=0; i<memSection->m_varCount; i++)
            m_varHash[memSection->m_dspVars[i].Name] = &(memSection->m_dspVars[i]);
    }
}

DspVarServer* DspVarResolver::getDspVar(const QString &varNameWithOffset)
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
    DspVarServer* dspVar = getDspVar(varNameWithOffset);
    if(dspVar) {
        ulong retoffs = dspVar->offs;
        QString offsetStr = extractOffset(varNameWithOffset, dspVar->Name);
        if(offsetStr.length() > 0) { // wenn noch was da, dann muss das ein +/- offset sein
            long offset = calcOffsetFromStr(offsetStr);
            if(offset < 0)
                return -1;
            retoffs += offset;
        }
        if (dspVar->segment == moduleGlobalSegment) // wenn daten im globalen segment liegen
            retoffs += (globalstartadr - userMemOffset);
        return retoffs;
    }
    // offset only e.g DSPMEMOFFSET
    return calcOffsetFromStr(varNameWithOffset);
}

long DspVarResolver::getVarAddress(const QString &varNameWithOffset)
{
    DspVarServer* dspVar = getDspVar(varNameWithOffset);
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
    DspVarServer* var = getDspVar(varNameWithOffset);
    if(var)
        return var->type;
    return dspDataTypeUnknown;
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
