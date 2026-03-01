#include "dspvarclientinterface.h"

int DspVarClientInterface::m_instanceCount = 0;

DspVarClientInterface::DspVarClientInterface(const QString &name, int size, DspDataType dataType, DspSegmentType dspSegmentType) :
    m_sName(name),
    m_dspSegmentType(dspSegmentType),
    m_dataType(dataType),
    m_dspVarData(size)
{
    m_instanceCount++;
}

DspVarClientInterface::~DspVarClientInterface()
{
    m_instanceCount--;
}

void DspVarClientInterface::setValue(int idx, float value)
{
    if(idx < 0 || idx>=m_dspVarData.size()) {
        qCritical("DspVar %s setValue %i out of limit", qPrintable(m_sName), idx);
        return;
    }
    m_dspVarData.replace(idx, value);
}

int DspVarClientInterface::getInstanceCount()
{
    return m_instanceCount;
}
