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
    quint32 edge;
    if ((edge = ((m_nValue ^ val) & val)) > 0)
        emit risingEdge(edge);
    m_nValue |= val;
}

void NotificationValue::clrValue(quint32 val)
{
    m_nValue = m_nValue & ~val;
}
