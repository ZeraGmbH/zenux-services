#ifndef TEST_AUTO_JOURNAL_LOGGER_H
#define TEST_AUTO_JOURNAL_LOGGER_H

#include <QObject>

class test_auto_journal_logger : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void manualLog();
    void periodicLog();
    void periodicLogAndStop();
};

#endif // TEST_AUTO_JOURNAL_LOGGER_H
