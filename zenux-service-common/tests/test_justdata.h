#ifndef TEST_JUSTDATA_H
#define TEST_JUSTDATA_H

#include "adjdataitemscpi.h"
#include "adjdatarange.h"
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
    void nodeSetReject();
    void coeffSetReject();
    void nodeSetFail();
    void coeffSetFail();
    void nodeSetOneCrap();
    void nodeSettwoCrap();
    void coefSetCrap();
private:
    cSCPI *scpi;
    AdjDataItemScpi *justData;
    AdjDataRange *m_adjDataGroup;
};

#endif // TEST_JUSTDATA_H
