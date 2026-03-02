#ifndef DSPVMEMORYSECTIONINTERNAL_H
#define DSPVMEMORYSECTIONINTERNAL_H

#include "dspvarinserver.h"
#include <QList>

class DspMemorySectionInternal
{
public:
    DspMemorySectionInternal() = default;
    DspMemorySectionInternal(long startAddress, int varCount, const DspVarInServer *dspVars);

    void clear();
    void appendDspVar(DspVarInServer &var);

    int getVarCount() const;
    DspVarServerPtr getDspVar(int varNum) const;

    long m_startAddress = 0;
private:
    QList<DspVarServerPtr> m_dspVarList;
};

#endif // DSPVARANDMEMSECTION_H
