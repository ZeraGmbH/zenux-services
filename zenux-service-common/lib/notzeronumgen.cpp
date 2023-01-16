#include "notzeronumgen.h"

NotZeroNumGen *NotZeroNumGen::m_theInstance = nullptr;

quint32 NotZeroNumGen::getMsgNr()
{
    if(!m_theInstance)
        m_theInstance = new NotZeroNumGen;
    return m_theInstance->getMsgNrFromObject();
}

quint32 NotZeroNumGen::getMsgNrFromObject()
{
    m_nMsgNr++;
    if (m_nMsgNr == 0)
        m_nMsgNr++;
    return m_nMsgNr;
}
