#ifndef TEST_CRITICALSTATUSMOCK_H
#define TEST_CRITICALSTATUSMOCK_H

#include <QObject>

class test_criticalstatusmock : public QObject
{
    Q_OBJECT
private slots:
    void resetCriticalStatus();
};

#endif // TEST_CRITICALSTATUSMOCK_H
