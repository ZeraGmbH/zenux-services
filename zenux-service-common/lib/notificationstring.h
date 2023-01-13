#ifndef NOTIFICATIONSTRING_H
#define NOTIFICATIONSTRING_H

#include <QObject>
#include <QString>

class NotificationString: public QObject
{
    Q_OBJECT

public:
    NotificationString();
    NotificationString(QString str);
    QString getString();
    void operator = (QString str);
    void forceTrigger();

signals:
    void valueChanged();

private:
    QString m_sValue;
};


#endif // NOTIFICATIONSTRING_H
