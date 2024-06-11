#ifndef TEST_FILEONOFFWATCHER_H
#define TEST_FILEONOFFWATCHER_H

#include <QObject>

class test_onofffilewatcher : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void initialOff();
    void initialOn();
    void dynamicOffOnOff();
};

#endif // TEST_FILEONOFFWATCHER_H
