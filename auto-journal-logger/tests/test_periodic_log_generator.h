#ifndef TEST_PERIODIC_LOG_GENERATOR_H
#define TEST_PERIODIC_LOG_GENERATOR_H

#include <QObject>

class test_periodic_log_generator : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void manualLog();
    void periodicLog();
    void periodicLogAndStop();
private:
    void deleteLogFile();

};

#endif // TEST_PERIODIC_LOG_GENERATOR_H
