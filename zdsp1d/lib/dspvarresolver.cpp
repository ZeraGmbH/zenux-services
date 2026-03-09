#include "dspvarresolver.h"
#include "dspapi.h"
#include "dspvaroffsetcalc.h"

DspVarResolver::DspVarResolver()
{
    m_varParser.SetDelimiter("(,+,-,)"); // setze die trennzeichen für den parser
    m_varParser.SetWhiteSpace(" (,)");
    m_varHash = DspStaticData::getVarHash();
}

void DspVarResolver::addSection(DspMemorySectionInternal* section)
{
    m_memSectionList.append(section);
}

void DspVarResolver::actualizeVarHash()
{
    m_varHash = DspStaticData::getVarHash();
    for(DspMemorySectionInternal* memSection : qAsConst(m_memSectionList)) {
        DspStaticData::initMemsection(memSection);
        for (int i=0; i<memSection->getVarCount(); i++) {
            DspVarServerPtr var = memSection->getDspVar(i);
            m_varHash[var->Name] = var;
        }
    }
}

DspVarServerPtr DspVarResolver::getDspVar(const QString &varNameWithOffset)
{
    QString upperName = varNameWithOffset.toUpper();
    const QChar* cts = upperName.data();
    QString baseVarName = m_varParser.GetKeyword(&cts);
    auto iter = m_varHash.constFind(baseVarName);
    if(iter != m_varHash.constEnd())
        return iter.value();
    return nullptr;
}

long DspVarResolver::getVarOffset(const QString& varNameWithOffset, ulong userMemOffset, ulong alignedStartAdr)
{
    DspVarServerPtr dspVar = getDspVar(varNameWithOffset);
    int calcedOffset = 0;
    if(dspVar) {
        if (!DspVarOffsetCalc::calcVarOffset(dspVar->Name, varNameWithOffset, calcedOffset))
            return -1;

        int retoffs = dspVar->offs + calcedOffset;
        if (dspVar->segment == moduleAlignedMemorySegment)
            retoffs += (alignedStartAdr - userMemOffset);
        return retoffs;
    }
    // offset only e.g DSPMEMOFFSET
    if (!DspVarOffsetCalc::calcVarOffset("", varNameWithOffset, calcedOffset))
        return -1;
    return calcedOffset;
}

long DspVarResolver::getVarAddress(const QString &varNameWithOffset)
{
    DspVarServerPtr dspVar = getDspVar(varNameWithOffset);
    int calcedOffset = 0;
    if(dspVar) {
        if (!DspVarOffsetCalc::calcVarOffset(dspVar->Name, varNameWithOffset, calcedOffset))
            return -1;
        return dspVar->adr + calcedOffset;
    }
    // offset only e.g DSPMEMOFFSET
    if (!DspVarOffsetCalc::calcVarOffset("", varNameWithOffset, calcedOffset))
        return -1;
    return calcedOffset;
}

int DspVarResolver::getVarType(const QString &varNameWithOffset)
{
    DspVarServerPtr var = getDspVar(varNameWithOffset);
    if(var)
        return var->type;
    return dspDataTypeUnknown;
}
