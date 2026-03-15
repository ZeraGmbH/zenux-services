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
        DspStaticData::initMemsection(memSection, 0);
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

long DspVarResolver::getVarOffset(const QString& varNameWithOffset)
{
    DspVarServerPtr dspVar = getDspVar(varNameWithOffset);
    if(dspVar) {
        int offsetToVar = 0; // varNameWithOffset: "<varname>+offset"
        if (!DspVarOffsetCalc::calcVarOffset(dspVar->Name, varNameWithOffset, offsetToVar))
            return -1;
        return dspVar->m_offsetToModuleBase + offsetToVar;
    }

    // offset only parameters e.g on DSPMEMOFFSET / STARTCHAIN...
    int offsetPassedIn = 0;
    if (!DspVarOffsetCalc::calcOffset(varNameWithOffset, offsetPassedIn))
        return -1;
    return offsetPassedIn;
}

long DspVarResolver::getVarAddress(const QString &varNameWithOffset)
{
    DspVarServerPtr dspVar = getDspVar(varNameWithOffset);
    if (dspVar == nullptr)
        return -1;

    int offsetToVar = 0; // varNameWithOffset: "<varname>+offset"
    // Should we really support offsets here?
    if (!DspVarOffsetCalc::calcVarOffset(dspVar->Name, varNameWithOffset, offsetToVar))
        return -1;
    return dspVar->m_absoluteAddress + offsetToVar;
}

int DspVarResolver::getVarType(const QString &varNameWithOffset)
{
    DspVarServerPtr var = getDspVar(varNameWithOffset);
    if(var)
        return var->type;
    return dspDataTypeUnknown;
}
