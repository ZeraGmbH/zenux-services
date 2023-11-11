#ifndef TEST_OLD_SCPI_IMPLEMENTATION_H
#define TEST_OLD_SCPI_IMPLEMENTATION_H

#include <QObject>

class test_old_scpi_implementation : public QObject
{
    Q_OBJECT
private slots:
    void query();
    void cmd();
};

#endif // TEST_OLD_SCPI_IMPLEMENTATION_H
