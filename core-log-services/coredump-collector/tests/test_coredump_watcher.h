#ifndef TEST_COREDUMP_WATCHER_H
#define TEST_COREDUMP_WATCHER_H

#include <QObject>

class test_coredump_watcher : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void test_single_core_file_ok();
    void test_multiple_core_file_ok();
    void test_dir_instead_of_dump_file();
    void test_only_filtered_uid_copy();
    void test_misnamed_core_file();
    void test_coredump_before_watcher_active();
private:
    bool writeTestCoreDump(int userId, QString programName);
};

#endif // TEST_COREDUMP_WATCHER_H
