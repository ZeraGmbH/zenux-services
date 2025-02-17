#include "dspvar.h"

void cDspVar::setValue(int idx, float value)
{
    if(idx < 0 || idx>=m_dspVarData.size()) {
        qCritical("DspVar %s setValue %i out of limit", qPrintable(m_sName), idx);
        return;
    }
    m_dspVarData.replace(idx, value);
}
