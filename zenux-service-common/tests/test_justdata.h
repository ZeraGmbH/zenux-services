#ifndef TEST_JUSTDATA_H
#define TEST_JUSTDATA_H

#include "justdata.h"

#include <scpi.h>
#include <QTest>

class test_justdata: public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void findScpiObject();
    void nodeSetAndRead();
    void coeffSetAndRead();

private:
    cSCPI *scpi;
    cJustData *justData;
};

#endif // TEST_JUSTDATA_H
