#ifndef TEST_COREDUMP_WATCHER_H
#define TEST_COREDUMP_WATCHER_H

#include <QObject>

class test_coredump_watcher : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void test_initial_find_one();
private:
    bool writeTestCoreDump(int userId);
};

#endif // TEST_COREDUMP_WATCHER_H
