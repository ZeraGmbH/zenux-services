#ifndef NOTIFICATIONVALUE_H
#define NOTIFICATIONVALUE_H

#include <QObject>
#include <QtGlobal>

class NotificationValue: public QObject
{
    Q_OBJECT

public:
    NotificationValue();
    NotificationValue(quint32 val);
    quint32 getValue();
    void setValue(quint32 val); // emits no signal
    void clrValue(quint32 val);

signals:
    void risingEdge(quint32);

private:
    quint32 m_nValue;
};


#endif // NOTIFICATIONVALUE_H
