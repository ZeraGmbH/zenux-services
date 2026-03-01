#include "dspmemorysectioninternal.h"

DspMemorySectionInternal::DspMemorySectionInternal(long startAddress, int varCount, DspVarServer *dspVars) :
    m_startAddress(startAddress),
    m_varCount(varCount),
    m_dspVars(dspVars)
{
}
