#ifndef TEST_BINSERIALIZE_H
#define TEST_BINSERIALIZE_H

#include <QObject>

class test_binserialize : public QObject
{
    Q_OBJECT
private slots:
    void serializeFloatToMicroController();
};

#endif // TEST_BINSERIALIZE_H
