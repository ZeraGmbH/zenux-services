#include "notificationstring.h"

NotificationString::NotificationString() :
    m_sValue("")
{
}

NotificationString::NotificationString(const QString &str) :
    m_sValue(str)
{
}

const QString &NotificationString::getString()
{
    return m_sValue;
}

void NotificationString::operator =(const QString &str)
{
    if(m_sValue != str) {
        m_sValue = str;
        emit valueChanged(m_sValue);
    }
}

void NotificationString::forceTrigger()
{
    emit valueChanged(m_sValue);
}
