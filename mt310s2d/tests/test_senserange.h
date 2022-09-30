#ifndef TEST_SENSERANGE_H
#define TEST_SENSERANGE_H

#include "senserangetesttemplate.h"
#include <QTest>

class test_senserange : public QObject, public SenseRangeTestTemplate
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void findScpiObject();
    void executeScpiQuery();
    void verifyScpiQuery();

};

#endif // TEST_SENSERANGE_H
