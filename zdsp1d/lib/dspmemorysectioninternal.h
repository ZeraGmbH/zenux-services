#ifndef DSPVMEMORYSECTIONINTERNAL_H
#define DSPVMEMORYSECTIONINTERNAL_H

#include "dspvarserver.h"

struct DspMemorySectionInternal {
    DspMemorySectionInternal() = default;
    DspMemorySectionInternal(long startAddress, int varCount, DspVarServer *dspVars);

    long m_startAddress = 0;
    int m_varCount = 0;
    DspVarServer *m_dspVars = nullptr;
};

#endif // DSPVARANDMEMSECTION_H
