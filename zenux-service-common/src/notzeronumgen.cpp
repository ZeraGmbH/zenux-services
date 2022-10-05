#include "notzeronumgen.h"

quint32 NotZeroNumGen::getMsgNr()
{
    m_nMsgNr++;
    if (m_nMsgNr == 0)
        m_nMsgNr++;
    return m_nMsgNr;
}
