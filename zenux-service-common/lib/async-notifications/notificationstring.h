#ifndef NOTIFICATIONSTRING_H
#define NOTIFICATIONSTRING_H

#include <QObject>
#include <QString>

class NotificationString: public QObject
{
    Q_OBJECT
public:
    NotificationString();
    explicit NotificationString(const QString &str);
    const QString &getString();
    void operator = (const QString &str);
    void forceTrigger();
signals:
    void valueChanged(const QString &newValue);

private:
    QString m_sValue;
};


#endif // NOTIFICATIONSTRING_H
