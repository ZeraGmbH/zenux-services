#include "dspvarresolver.h"
#include "dspapi.h"
#include "dspvaroffsetcalc.h"

DspVarResolver::DspVarResolver() :
    m_varHash(DspStaticData::getVarHash())
{
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
    upperName.remove(" ");

    const QStringList summands = upperName.split("+", Qt::SkipEmptyParts);
    if (summands.isEmpty())
        return nullptr;

    const QString baseVarName = summands[0];
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
    // offset only parameters e.g on DSPMEMOFFSET / STARTCHAIN...
    if (!DspVarOffsetCalc::calcOffset(varNameWithOffset, calcedOffset))
        return -1;
    return calcedOffset;
}

long DspVarResolver::getVarAddress(const QString &varNameWithOffset)
{
    DspVarServerPtr dspVar = getDspVar(varNameWithOffset);
    if (dspVar == nullptr)
        return -1;

    int calcedOffset = 0;
    // Should we really support offsets here?
    if (!DspVarOffsetCalc::calcVarOffset(dspVar->Name, varNameWithOffset, calcedOffset))
        return -1;
    return dspVar->adr + calcedOffset;
}

int DspVarResolver::getVarType(const QString &varNameWithOffset)
{
    DspVarServerPtr var = getDspVar(varNameWithOffset);
    if(var)
        return var->type;
    return dspDataTypeUnknown;
}
