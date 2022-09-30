#ifndef TEST_SENSERANGE_H
#define TEST_SENSERANGE_H

#include <QTest>

class test_senserange : public QObject
{
    Q_OBJECT
private slots:
    void findScpiObject();
    void executeScpiQuery();
    void verifyScpiQuery();

};

#endif // TEST_SENSERANGE_H
