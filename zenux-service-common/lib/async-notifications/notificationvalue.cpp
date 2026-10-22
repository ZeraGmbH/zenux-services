#include "notificationvalue.h"

NotificationValue::NotificationValue()
    :m_nValue(0)
{
}

NotificationValue::NotificationValue(quint32 val)
    :m_nValue(val)
{
}

quint32 NotificationValue::getValue()
{
    return m_nValue;
}

void NotificationValue::setValue(quint32 val)
{
    const quint32 changedBitMask = (m_nValue ^ val);
    const quint32 risingEdgeBits = changedBitMask & val;
    if (risingEdgeBits > 0)
        emit risingEdge(risingEdgeBits, this);
    m_nValue |= val;
}

void NotificationValue::clrValue(quint32 val)
{
    m_nValue = m_nValue & ~val;
}
