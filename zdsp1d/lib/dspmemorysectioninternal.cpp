#include "dspmemorysectioninternal.h"

DspMemorySectionInternal::DspMemorySectionInternal(long startAddress, int varCount, const DspVarInServer *dspVars) :
    m_startAddress(startAddress)
{
    for(int i=0; i<varCount; ++i)
        m_dspVarList.append(std::make_shared<DspVarInServer>(dspVars[i]));
}

void DspMemorySectionInternal::clear()
{
    m_dspVarList.clear();
}

void DspMemorySectionInternal::appendDspVar(DspVarInServer &var)
{
    m_dspVarList.append(std::make_shared<DspVarInServer>(var));
}

int DspMemorySectionInternal::getVarCount() const
{
    return m_dspVarList.count();
}

DspVarServerPtr DspMemorySectionInternal::getDspVar(int varNum) const
{
    return m_dspVarList[varNum];
}
