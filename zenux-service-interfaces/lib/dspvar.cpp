#include "dspvar.h"

int cDspVar::m_instanceCount = 0;

cDspVar::cDspVar(const QString &name, int size, int type, int datatype ) :
    m_sName(name),
    m_nType(type),
    m_nDataType(datatype),
    m_dspVarData(size)
{
    m_instanceCount++;
}

cDspVar::~cDspVar()
{
    m_instanceCount--;
}

void cDspVar::setValue(int idx, float value)
{
    if(idx < 0 || idx>=m_dspVarData.size()) {
        qCritical("DspVar %s setValue %i out of limit", qPrintable(m_sName), idx);
        return;
    }
    m_dspVarData.replace(idx, value);
}

int cDspVar::getInstanceCount()
{
    return m_instanceCount;
}
