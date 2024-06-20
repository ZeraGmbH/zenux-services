#ifndef TEST_CMD_HANDLER_H
#define TEST_CMD_HANDLER_H

#include <QObject>

class test_cmd_handler : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void filterCoreDumpFilesByUid();
};

#endif // TEST_CMD_HANDLER_H
