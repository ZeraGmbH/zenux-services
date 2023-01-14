#include "notificationstring.h"

NotificationString::NotificationString()
    :m_sValue("")
{
}

NotificationString::NotificationString(QString str)
    :m_sValue(str)
{
}

QString NotificationString::getString()
{
    return m_sValue;
}

void NotificationString::operator =(QString str)
{
    if (m_sValue != str)
        emit valueChanged();
    m_sValue = str;
}

void NotificationString::forceTrigger()
{
    emit valueChanged();
}
